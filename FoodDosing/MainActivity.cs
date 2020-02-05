namespace FoodDosing
{
    using System;
    using System.Text;

    using Android.App;
    using Android.Content;
    using Android.Nfc;
    using Android.Nfc.Tech;
    using Android.OS;
    using Android.Util;
    using Android.Views;
    using Android.Widget;
    using Android.Text.Format;

    using Java.IO;
    using System.Collections.Generic;
    using System.Linq;

    [Activity(Label = "@string/app_name", MainLauncher = true, Icon = "@drawable/icon")]
    public class MainActivity : Activity
    {
        
        public const string ViewApeMimeType = "application/vnd.xamarin.FoodDosing";
        public static readonly string NfcAppRecord = "xamarin.FoodDosing";
        public static readonly string Tag = "FoodDosing";
        //public const string ViewTextType = "RTD_TEXT";
        //public static byte[] RTD_TEXT;
        public const string la = "RTD_TEXT";

        private bool _inWriteMode;
        private NfcAdapter _nfcAdapter;
        private TextView _textView;
        private Button _writeTagButton;

        private TextView timeDisplay1;
        private Button timeSelectButton1;
        private TextView timeDisplay2;
        private Button timeSelectButton2;
        private TextView timeDisplay3;
        private Button timeSelectButton3;
        private TextView timeDisplay4;
        private Button timeSelectButton4;

        
        private int payload_size = 0;


        protected override void OnCreate(Bundle bundle)
        {
            base.OnCreate(bundle);
            SetContentView(Resource.Layout.Main);

            // Get a reference to the default NFC adapter for this device. This adapter 
            // is how an Android application will interact with the actual hardware.
            _nfcAdapter = NfcAdapter.GetDefaultAdapter(this);

            timeDisplay1 = FindViewById<TextView>(Resource.Id.time_display1);
            timeSelectButton1 = FindViewById<Button>(Resource.Id.select_button1);
            timeDisplay2 = FindViewById<TextView>(Resource.Id.time_display2);
            timeSelectButton2 = FindViewById<Button>(Resource.Id.select_button2);
            timeDisplay3 = FindViewById<TextView>(Resource.Id.time_display3);
            timeSelectButton3 = FindViewById<Button>(Resource.Id.select_button3);
            timeDisplay4 = FindViewById<TextView>(Resource.Id.time_display4);
            timeSelectButton4 = FindViewById<Button>(Resource.Id.select_button4);

            timeSelectButton1.Click += TimeSelectOnClick;
            timeSelectButton2.Click += TimeSelectOnClick;
            timeSelectButton3.Click += TimeSelectOnClick;
            timeSelectButton4.Click += TimeSelectOnClick;


            _writeTagButton = FindViewById<Button>(Resource.Id.write_tag_button);
            _writeTagButton.Click += WriteTagButtonOnClick;

            _textView = FindViewById<TextView>(Resource.Id.text_view);



        }

        /// <summary>
        /// This method is called when an NFC tag is discovered by the application.
        /// </summary>
        /// <param name="intent"></param>
        protected override void OnNewIntent(Intent intent)
        {
            if (_inWriteMode)
            {
                _inWriteMode = false;
                var tag = intent.GetParcelableExtra(NfcAdapter.ExtraTag) as Tag;

                if (tag == null)
                {
                    return;
                }

                //Validating alarm time

                //variable to store data, later send by nfc to nucleo device
                var preparePayload = "";
                //erasing : to sort without problem
                List<int> timeList = new List<int>();
                int helper=0;
                payload_size = 0;
                if (timeDisplay1.Text != "Pick time of first feeding")
                {
                    //skipping ":"
                    //converting value from string to int
                    helper  = 1000*(Convert.ToInt32(timeDisplay1.Text[0])-48);
                    helper += 100*(Convert.ToInt32(timeDisplay1.Text[1]) - 48);
                    helper += 10*(Convert.ToInt32(timeDisplay1.Text[3]) - 48);
                    helper +=     Convert.ToInt32(timeDisplay1.Text[4]) - 48;
                   

                   
                    timeList.Add(helper);
                    helper = 0;
                    payload_size += 1;
                }

                if (timeDisplay2.Text != "Pick time of second feeding")
                {
                    helper = 1000 * (Convert.ToInt32(timeDisplay2.Text[0]) - 48);
                    helper += 100 * (Convert.ToInt32(timeDisplay2.Text[1]) - 48);
                    helper += 10 * (Convert.ToInt32(timeDisplay2.Text[3]) - 48);
                    helper += Convert.ToInt32(timeDisplay2.Text[4]) - 48;

                    timeList.Add(helper);
                    helper = 0;
                    payload_size += 1;
                }

                if (timeDisplay3.Text != "Pick time of third feeding")
                {
                    helper = 1000 * (Convert.ToInt32(timeDisplay3.Text[0]) - 48);
                    helper += 100 * (Convert.ToInt32(timeDisplay3.Text[1]) - 48);
                    helper += 10 * (Convert.ToInt32(timeDisplay3.Text[3]) - 48);
                    helper += Convert.ToInt32(timeDisplay3.Text[4]) - 48;

                    timeList.Add(helper);
                    helper = 0;
                    payload_size += 1;
                }

                if (timeDisplay4.Text != "Pick time of last feeding")
                {
                    helper = 1000 * (Convert.ToInt32(timeDisplay4.Text[0]) - 48);
                    helper += 100 * (Convert.ToInt32(timeDisplay4.Text[1]) - 48);
                    helper += 10 * (Convert.ToInt32(timeDisplay4.Text[3]) - 48);
                    helper += Convert.ToInt32(timeDisplay4.Text[4]) - 48;

                    timeList.Add(helper);
                    helper = 0;
                    payload_size += 1;
                }

                //Sorting
                timeList.Sort();

                
                int current_time = (100*DateTime.Now.Hour) + DateTime.Now.Minute;//fix that
                //finding first alarm
                //int nearest = timeList.OrderBy(x => Math.Abs((long)x - current_time)).First();
                int nearest = 0;
                for(int i=0;i<timeList.Count;i++)
                {
                    if(timeList[i]>current_time)
                    {
                        nearest = timeList[i];
                        break;
                    }
                }
                
                //creating new list for the expected order
                List<int> order = new List<int>();

                //adding first alarm
                order.Add(nearest);
               
                for(int h=0;h<timeList.Count();h++)
                {
                    //checking to not write the same value twice to the list
                    if (timeList[h]!=order[0])
                    {
                        //timeList is sort in ascending order
                        if (order[0] < timeList[h])
                        {
                            order.Add(timeList[h]);
                        }
                    }
                }
                //adding the alarms that will be next day, becose today it's too late for them
                for (int h = 0; h < timeList.Count(); h++)
                {
                    //checking to not write the same value twice to the list
                    if (timeList[h] != order[0])
                    {
                        if (order[0] > timeList[h])
                        {
                            order.Add(timeList[h]);
                        }
                    }
                }

                //writing data from list to variable created for storing data
                for(int i=0;i<order.Count;i++)
                {
                    preparePayload += order[i];
                    preparePayload += ",";
                }


                //When user didn't set any alarm, then automatically set four alarms on 11,12,13,14
                if ((timeDisplay1.Text == "Pick time of first feeding") && (timeDisplay2.Text == "Pick time of second feeding") && (timeDisplay3.Text != "Pick time of third feeding") && (timeDisplay4.Text != "Pick time of last feeding"))
                {
                    preparePayload = "11,12,13,14";
                    payload_size = 4;
                }


                // These next few lines will create a payload (consisting of a string)
                // and a mimetype. NFC record are arrays of bytes. 
                var payload = Encoding.ASCII.GetBytes(DateTime.Now +","+ payload_size+","+preparePayload);
                
                var mimeBytes = Encoding.ASCII.GetBytes(ViewApeMimeType);
                
                var textBytes = Encoding.ASCII.GetBytes(la);//erase
                
                
                var ndefRecord = new NdefRecord(NdefRecord.TnfMimeMedia,mimeBytes , new byte[0], payload);
                

                var ndefMessage = new NdefMessage(new NdefRecord[] { ndefRecord }); 
                
                
                if (!TryAndWriteToTag(tag, ndefMessage))
                {
                    // Maybe the write couldn't happen because the tag wasn't formatted?
                    TryAndFormatTagWithMessage(tag, ndefMessage);                    
                }
            }
        }

        protected override void OnPause()
        {
            base.OnPause();
            // App is paused, so no need to keep an eye out for NFC tags.
			if (_nfcAdapter != null)
            	_nfcAdapter.DisableForegroundDispatch(this);
        }

        private void DisplayMessage(string message)//message
        {
            _textView.Text = message;
            Log.Info(Tag, message);


            
        }

        /// <summary>
        /// Identify to Android that this activity wants to be notified when 
        /// an NFC tag is discovered. 
        /// </summary>
        private void EnableWriteMode()
        {
            _inWriteMode = true;

            // Create an intent filter for when an NFC tag is discovered.  When
            // the NFC tag is discovered, Android will u
            var tagDetected = new IntentFilter(NfcAdapter.ActionTagDiscovered);
            var filters = new[] { tagDetected };
            
            // When an NFC tag is detected, Android will use the PendingIntent to come back to this activity.
            // The OnNewIntent method will invoked by Android.
            var intent = new Intent(this, GetType()).AddFlags(ActivityFlags.SingleTop);
            var pendingIntent = PendingIntent.GetActivity(this, 0, intent, 0);

			if (_nfcAdapter == null) {
				var alert = new AlertDialog.Builder (this).Create ();
				alert.SetMessage ("NFC is not supported on this device.");
				alert.SetTitle ("NFC Unavailable");
				alert.SetButton ("OK", delegate {
					_writeTagButton.Enabled = false;
					_textView.Text = "NFC is not supported on this device.";
				});
				alert.Show ();
			} else
            	_nfcAdapter.EnableForegroundDispatch(this, pendingIntent, filters, null);
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="tag"></param>
        /// <param name="ndefMessage"></param>
        /// <returns></returns>
        private bool TryAndFormatTagWithMessage(Tag tag, NdefMessage ndefMessage)
        {
            var format = NdefFormatable.Get(tag);
            if (format == null)
            {
                DisplayMessage("Tag does not appear to support NDEF format.");
            }
            else
            {
                try
                {
                    format.Connect();
                    format.Format(ndefMessage);
                    DisplayMessage("Tag successfully written.");
                    return true;
                }
                catch (IOException ioex)
                {
                    var msg = "There was an error trying to format the tag.";
                    DisplayMessage(msg);
                    Log.Error(Tag, ioex, msg);
                }
            }
            return false;
        }

       

        private void WriteTagButtonOnClick(object sender, EventArgs eventArgs)
        {
            var view = (View)sender;
            if (view.Id == Resource.Id.write_tag_button)
            {
                DisplayMessage("Touch and hold the tag against the phone to write.");
                EnableWriteMode();
            }
        }

        /// <summary>
        /// This method will try and write the specified message to the provided tag. 
        /// </summary>
        /// <param name="tag">The NFC tag that was detected.</param>
        /// <param name="ndefMessage">An NDEF message to write.</param>
        /// <returns>true if the tag was written to.</returns>
        private bool TryAndWriteToTag(Tag tag, NdefMessage ndefMessage)
        {

            // This object is used to get information about the NFC tag as 
            // well as perform operations on it.
            var ndef = Ndef.Get(tag); 
            if (ndef != null)
            {
                ndef.Connect();

                // Once written to, a tag can be marked as read-only - check for this.
                if (!ndef.IsWritable)
                {
                    DisplayMessage("Tag is read-only.");
                }

                // NFC tags can only store a small amount of data, this depends on the type of tag its.
                var size = ndefMessage.ToByteArray().Length;
                if (ndef.MaxSize < size)
                {
                    DisplayMessage("Tag doesn't have enough space.");
                }
               
                ndef.WriteNdefMessage(ndefMessage);
                DisplayMessage("Succesfully wrote tag.");
                return true;
            }

            return false;
        }
        public class TimePickerFragment : DialogFragment, TimePickerDialog.IOnTimeSetListener
        {
            public static readonly string TAG = "MyTimePickerFragment";
            Action<DateTime> timeSelectedHandler = delegate { };

            public static TimePickerFragment NewInstance(Action<DateTime> onTimeSelected)
            {
                TimePickerFragment frag = new TimePickerFragment();
                frag.timeSelectedHandler = onTimeSelected;
                return frag;
            }

            public override Dialog OnCreateDialog(Bundle savedInstanceState)
            {
                DateTime currentTime = DateTime.Now;
                bool is24HourFormat = DateFormat.Is24HourFormat(Activity);
                TimePickerDialog dialog = new TimePickerDialog
                    (Activity, this, currentTime.Hour, currentTime.Minute, is24HourFormat);
                return dialog;
            }

            public void OnTimeSet(TimePicker view, int hourOfDay, int minute)
            {
                DateTime currentTime = DateTime.Now;
                DateTime selectedTime = new DateTime(currentTime.Year, currentTime.Month, currentTime.Day, hourOfDay, minute, 0);
                Log.Debug(TAG, selectedTime.ToLongTimeString());
                timeSelectedHandler(selectedTime);
            }
        }

        void TimeSelectOnClick(object sender, EventArgs eventArgs)
        {
            TimePickerFragment frag = TimePickerFragment.NewInstance(
                delegate (DateTime time)
                {
                    //proper assignment of functions, chosen button to display
                    if (sender.Equals(FindViewById<Button>(Resource.Id.select_button1)))
                    {
                        timeDisplay1.Text = time.ToShortTimeString();
                        
                    }
                    else if(sender.Equals(FindViewById<Button>(Resource.Id.select_button2)))
                    {
                        timeDisplay2.Text = time.ToShortTimeString();
                        
                    }
                    else if (sender.Equals(FindViewById<Button>(Resource.Id.select_button3)))
                    {
                        timeDisplay3.Text = time.ToShortTimeString();
                        
                    }
                    else if (sender.Equals(FindViewById<Button>(Resource.Id.select_button4)))
                    {
                        timeDisplay4.Text = time.ToShortTimeString();
                        
                    }

                });

            frag.Show(FragmentManager, TimePickerFragment.TAG);
        }

    }
}
