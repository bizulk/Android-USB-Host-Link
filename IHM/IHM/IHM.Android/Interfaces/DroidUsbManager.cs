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

        public DroidUsbManager(UsbManager usbManager)
        {
            usbManager_ = usbManager;
        }

        public int Init()
        {
            /* TODO : Do Droid Stuff */
            // Je mets -1 exprès pour mettre en evidence le fait que l'on a pas encore fait le taff
            return -1;
        }

        public ICollection<string> getListOfConnections()
        {
            return usbManager_.DeviceList.Keys;
        }

        public int getDeviceConnection(string name)
        {
            return usbManager_.OpenDevice(((Dictionary<string, UsbDevice>)usbManager_.DeviceList)[name]).FileDescriptor;
        }

        public int Close()
        {
            /* TODO : Do Droid Stuff */
            return 0;
        }
    }
}