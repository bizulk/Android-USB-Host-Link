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

/// <summary>
/// NON WORKING USB INTERFACE. Keep for hisstory log
/// This interface was developped by telecom student
///     - It only uses the Android API USB.
///     - It manages to list the connected device, displaying the system path, ex : /dev/bus/usb/001/004
///     - I'm not sure the file descriptor is valid.
/// </summary>
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

        public DevHandle getDeviceConnection()
        {
            DevHandle devHandle = new DevHandle();
            if (!selectedDevice.Equals("null"))
            {
                devHandle.fd = usbManager_.OpenDevice(((Dictionary<string, UsbDevice>)usbManager_.DeviceList)[selectedDevice]).FileDescriptor;
            }
            return devHandle;
        }

        public int Close()
        {
            return 0;
        }

        public int ReadRegisterFromDevice(byte uiRegister, ref byte value)
        {
            // not implemented
            return -1;
        }

        public int WriteRegisterToDevice(byte uiRegister, byte value)
        {
            // not implemented
            return -1;
        }

        public int WriteToDevice(byte[] data)
        {
            // not implemented
            return -1;
        }
        public int ReadFromDevice(ref byte[] data, int len)
        {
            // not implemented
            return -1;
        }
}
}