using System;
using System.Collections.Generic;
using System.Text;

namespace IHM
{
    /// <summary>
    /// this is a wrapper generated of the dllComm library.
    /// </summary>
    /// 
    /* vu comme c'est crée : je pense qu'il faut déifnir une IProtoIFace, et dll_if l'instancie
     Et elle contient son instance de protocomm.
     Sinon pour le singleton, pas obligatoire peut-être pour cette appli, et le "lazy" ça peut marcher (moins lourd a lire mais tu as peut-être un avis là dessus ?)
         */
    public sealed class dll_if 
    {
        private static dll_if instance = null;
        private static readonly object Instancelock = new object();
        // Handle vers le panda
        private proto_hdle_t m_handle;
        public const int _iLogGlobalNbMsg = 30;
        //Device (exception si on initialise à NULL)
        proto_IfaceIODevice_t m_device;

        public static dll_if GetInstance
        {
            get
            {
                if (instance == null)
                {
                    lock (Instancelock)
                    {
                        if (instance == null)
                        {
                            instance = new dll_if();
                        }

                    }
                }
                return instance;
            }
        }

        dll_if()
        {
            protocomm.log_global_create(_iLogGlobalNbMsg);
        }

        ~dll_if()
        {
            protocomm.log_global_destroy();
        }

        /// <summary>
        /// Initialize library
        /// </summary>
        /// <param name="device"> device pour la connexion </param>
        /// <param name="szPath"> path à passer au protocole </param>
        /// <returns> 0 if OK, otherwise < 0</returns>
        public int Open(proto_IfaceIODevice_t device, string szPath)
        {
            int ret = 0;

            m_device = device;

            // On cree l'instance de protocole
            m_handle = protocomm.proto_master_create(m_device); //5s de timeout TODO a moditifer plus tard pour le rendre paramètrable
            //On vérifie qu'il n'y a pas eu de problème lors de la connexion
            if(m_handle == null)
            {
                return -1;
            }
            ret = protocomm.proto_master_open(m_handle, szPath);
            if (ret != 0)
            {
                return -1;
            }

            return ret;
        }

        /// <summary>
        /// Fermeture de la connexion
        /// </summary>
        /// <returns> 0 si pas d'erreur et autre si une erreur </returns>
        public void Close()
        {
            //On ferme la connexion
            protocomm.proto_master_close(m_handle);
            protocomm.proto_master_destroy(m_handle);
            //On met l'handle à null pour signifier qu'on s'est déconnecté
            m_handle = null;
        }

        /// <summary>
        /// Ecriture de la valeur dans le registre
        /// </summary>
        /// <param name="uiRegister"> Registre à écrire </param>
        /// <param name="uiValue"> Valeur à écrire </param>
        /// <returns></returns>
        public proto_Status_t WriteRegister(byte uiRegister, byte uiValue)
        {
            proto_Status_t ret;

            ret = protocomm.proto_master_set(m_handle, uiRegister, uiValue);

            return ret;
        }

        /// <summary>
        /// Lecture de la valeur du registre
        /// </summary>
        /// <param name="uiRegister"> Registre à lire </param>
        /// <param name="value"> Valeur à lire </param>
        /// <returns></returns>
        public proto_Status_t ReadRegister(byte uiRegister, ref byte value)
        {
            proto_Status_t ret;
            // SLI Je pige pas que SWIG ait crée un type opaque por l'uint8_t ??
            var uiValue = protocomm.new_uint8_t_p();

            ret = protocomm.proto_master_get(m_handle, uiRegister, uiValue);

            //On récupère la valeur
            value = protocomm.uint8_t_p_value(uiValue);

            return ret;
        }

        /// <summary>
        /// Return an instance of the selected device
        /// </summary>
        /// <returns> Instance de notre device </returns>
        public proto_IfaceIODevice_t CreateDevice(proto_iodev_devices_t type)
        {
            proto_IfaceIODevice_t ret;
            ret = protocomm.device_create(type);
            return ret;
        }

        public int SerialSetFd(proto_IfaceIODevice_t dev, DevHandle devh)
        {
            int ret = protocomm.devserial_setFD(dev, devh.fd);
            return ret;
        }

        public int UsbDevSetFd(proto_IfaceIODevice_t dev, DevHandle devh)
        {
            int ret = protocomm.devusbdev_setDev(dev, devh.fd, devh.ep_in, devh.ep_out, devh.max_pkt_size);
            return ret;
        }


        public int LibUsbSetFd(proto_IfaceIODevice_t dev, DevHandle devh)
        {
            int ret = protocomm.devlibusb_setFD(dev, devh.fd, devh.ep_in, devh.ep_out, devh.max_pkt_size);
            return ret;
        }

        /// <summary>
        /// Fournit une string de description du status
        /// </summary>
        /// <param name="status"> Status à interpreter</param>
        public static string ProtoStatusGetString(proto_Status_t status)
        {
            List<string> lszStatus = new List<string>
            {
                  "Success",
                  "Erreur Système",
                  "CRC de la trame reçue invalide",
                  "Le peer signale une erreur de CRC",
                  "Argument invalide",
                  "timeout",
                  "Erreur de protocole"
            };
            return lszStatus[(int)status];
        }

    }
}
