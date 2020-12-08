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

        //Device (exception si on initialise à NULL)
        SWIGTYPE_p_proto_Device_t m_device;

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

        /// <summary>
        /// Initialize library
        /// </summary>
        /// <param name="device"> device pour la connexion </param>
        /// <param name="szPath"> path à passer au protocole </param>
        /// <returns> 0 if OK, otherwise < 0</returns>
        public int Open(SWIGTYPE_p_proto_Device_t device, string szPath)
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

            ret = protocomm.log_global_create(10);
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
            // Réinitialisation du log
            protocomm.log_global_destroy();
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
        /// Retourne une instance de device d'émulation de slave
        /// </summary>
        /// <returns> Instance de notre device </returns>
        public SWIGTYPE_p_proto_Device_t CreateEmulslave()
        {
            SWIGTYPE_p_proto_Device_t ret;
            ret = protocomm.devemulslave_create();
            return ret;
        }

        /// <summary>
        /// Creation du device serial
        /// Pour démonstration de l'utilisation de l'interface Android
        /// </summary>
        /// <returns></returns>
        public SWIGTYPE_p_proto_Device_t CreateDevSerial()
        {
            SWIGTYPE_p_proto_Device_t dev;
            dev = protocomm.devserial_create();
            return dev;
        }

        public int SerialSetFd(SWIGTYPE_p_proto_Device_t dev, DevHandle devh)
        {
            int ret = protocomm.devserial_setFD(dev, devh.fd);
            return ret;
        }

        public SWIGTYPE_p_proto_Device_t CreateDevUsbDev()
        {
            SWIGTYPE_p_proto_Device_t dev;
            dev = protocomm.devusbdev_create();
            return dev;
        }

        public int UsbDevSetFd(SWIGTYPE_p_proto_Device_t dev, DevHandle devh)
        {
            int ret = protocomm.devusbdev_setDev(dev, devh.fd, devh.ep_in, devh.ep_out, devh.max_pkt_size);
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
