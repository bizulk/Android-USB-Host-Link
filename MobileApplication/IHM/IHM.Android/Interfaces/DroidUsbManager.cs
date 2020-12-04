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
using System.Threading.Tasks;

namespace IHM.Droid.Interfaces
{
    public class DroidUsbManager : IUsbManager
    {
        private UsbManager usbManager_;
        private string selectedDevice;

        public DroidUsbManager() {}

        public void Init(Object context)
        {
            usbManager_ = (UsbManager)((ContextWrapper)context).GetSystemService(Context.UsbService);
        }

        public ICollection<string> getListOfConnections()
        {
            return usbManager_.DeviceList.Keys;
        }

        public Task<ICollection<string>> getListOfConnectionsAsync()
        {
            // On fait la meme chose
            return new Task<ICollection<string>> (getListOfConnections);
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