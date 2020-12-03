using System;

using Android.App;
using Android.Content.PM;
using Android.Runtime;
using Android.Views;
using Android.Widget;
using Android.OS;
using Android.Hardware.Usb;

using IHM.Droid.Interfaces;
using Android.Content;

namespace IHM.Droid
{
    [Activity(Label = "IHM", Icon = "@mipmap/icon", Theme = "@style/MainTheme", MainLauncher = true, ConfigurationChanges = ConfigChanges.ScreenSize | ConfigChanges.Orientation)]
    public class MainActivity : Xamarin.Forms.Platform.Android.FormsAppCompatActivity
    {
        protected override void OnCreate(Bundle savedInstanceState)
        {
            //Pour l'utilisation du service de l'USBManager
            Xamarin.Forms.DependencyService.Register<DroidPandaVcom>();

            TabLayoutResource = Resource.Layout.Tabbar;
            ToolbarResource = Resource.Layout.Toolbar;

            base.OnCreate(savedInstanceState);

            //Fichier de log
            Plugin.XFLogger.CrossXFLogger.Current.Configure(Plugin.XFLogger.Abstractions.LogTimeOption.DateTimeNow, "FileLog.log", 3, 1024, Plugin.XFLogger.Abstractions.LogLevel.Info, true);

            //Pour l'utilisation du service de partage
            Xamarin.Forms.DependencyService.Register<DroidShareService>();

            //Pour l'utilisation du service de téléchargement et de partage, récupération du path où enregistrer le fichier
            Xamarin.Forms.DependencyService.Register<DroidLocalStoragePath>();

            Xamarin.Forms.Forms.Init(this, savedInstanceState);
            Xamarin.Forms.DependencyService.Get<IUsbManager>().Init(this);
            LoadApplication(new App());

        }
    }
}
