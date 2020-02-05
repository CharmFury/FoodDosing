namespace FoodDosing
{
    using System;
    using System.Text;

    using Android.App;
    using Android.Nfc;
    using Android.OS;
    using Android.Widget;
    using Android.Util;
    
    [Activity, IntentFilter(new[] { "android.nfc.action.NDEF_DISCOVERED" }, 
        Categories = new[] { "android.intent.category.DEFAULT" })]// DataMimeType = MainActivity.ViewApeMimeType, Categories = new[] { "android.intent.category.DEFAULT" }
    public class DisplayHominidActivity : Activity
    {
        private ImageView _imageView;

        private TextView nfcText;

        protected override void OnCreate(Bundle savedInstanceState)
        {
            base.OnCreate(savedInstanceState);
            SetContentView(Resource.Layout.DisplayHominid);
            if (Intent == null)
            {
                return;
            }

            var intentType = Intent.Type ?? String.Empty;
            

            var button = FindViewById<Button>(Resource.Id.back_to_main_activity);
            button.Click += (sender, args) => Finish();


            nfcText = FindViewById<TextView>(Resource.Id.tv_nfcText);
            
                // Get the string that was written to the NFC tag, and display it.
                var rawMessages = Intent.GetParcelableArrayExtra(NfcAdapter.ExtraNdefMessages);
                var msg = (NdefMessage)rawMessages[0];

                var ndefRecord1 = msg.GetRecords()[0];
                var ndefMessage1 = Encoding.ASCII.GetString(ndefRecord1.GetPayload());


                DisplayTagAction(ndefMessage1);
            //}
        }


        /// <summary>
		/// Displays the tag action.
		/// </summary>
		/// <param name="ndefMessage">Ndef message.</param>
		private void DisplayTagAction(string ndefMessage)
        {



            nfcText.Text = ndefMessage;
            Toast.MakeText(this, ndefMessage, ToastLength.Long).Show();
            Log.Info(MainActivity.Tag, ndefMessage);




        }
    }
}
  
