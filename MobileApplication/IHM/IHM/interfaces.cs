using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Text;
using System.Threading.Tasks;

namespace IHM
{
    /// <summary>
    /// This Interface defines our USB Interface implémentation
    /// </summary>
    public interface IUsbManager
    {
        /// <summary>
        /// Some custom Initialization
        /// </summary>
        /// <returns> pourquoi pas le FD, pour l'instant je fais comme ça</returns>
        void Init(Object context);

        /// <summary>
        /// Retrieve all the USB corrently connected devices
        /// </summary>
        /// <returns> une liste du nom de chaque connexion </returns>
        ICollection<string> getListOfConnections();

        /// <summary>
        /// Same But with async method, allow to wait for user to connect the device
        /// </summary>
        /// <returns></returns>
       Task<ICollection<string>> getListOfConnectionsAsync();

        /// <summary>
        /// Select device from the the connected list. After this call the device is ready for communication
        /// </summary>
        /// <returns></returns>
        void selectDevice(string name);

        /// <summary>
        /// Returns a system File descriptor for the opened connection
        /// </summary>
        /// <returns>Le FD</returns>
        int getDeviceConnection();

        /// <summary>
        /// Closes all currently opened connection.
        /// </summary>
        /// <returns></returns>
        int Close();
    }

    /// <summary>
    /// Get Storage local path
    /// </summary>
    public interface ILocalStoragePath
    {
        string GetLocalStoragePath();
    }

    /// <summary>
    /// Call the system share service
    /// </summary>
    public interface IShareService
    {
        bool Share(string filename, string title);
    }

}
