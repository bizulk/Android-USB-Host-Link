using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;

using Android.App;
using Android.Content;
using Android.OS;
using Android.Runtime;
using Android.Views;
using Android.Widget;
using Android.Hardware.Usb;

using IHM;


namespace IHM.Droid.Interfaces
{
    public class DroidUsbManager : IUsbManager
    {
        private UsbManager usbManager_;
        private string selectedDevice;

        public DroidUsbManager() {}

        public void Init(Object context)
        {
            /* TODO : Do Droid Stuff */
            // Je mets -1 exprès pour mettre en evidence le fait que l'on a pas encore fait le taff
            usbManager_ = (UsbManager)((ContextWrapper)context).GetSystemService(Context.UsbService);
        }

        public ICollection<string> getListOfConnections()
        {
            return usbManager_.DeviceList.Keys;
        }

        public void selectDevice(string name)
        {
            if (usbManager_.DeviceList.ContainsKey(name)) 
            {
                selectedDevice = name;
            }
            else // Normalement impossible sauf si l'appareil s'est deconnecté entre la sélection de son nom et le moment d'appuie sur Valider
            {
                selectedDevice = "null";
            }
        }

        public int getDeviceConnection()
        {
            if (selectedDevice.Equals("null"))
                return -1;
            return usbManager_.OpenDevice(((Dictionary<string, UsbDevice>)usbManager_.DeviceList)[selectedDevice]).FileDescriptor;
        }

        public int Close()
        {
            return 0;
        }
    }
}