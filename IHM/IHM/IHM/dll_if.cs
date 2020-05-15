using System;
using System.Collections.Generic;
using System.Text;

namespace IHM
{
    public sealed class dll_if : protocomm
    {
        private static dll_if instance = null;
        private static readonly object Instancelock = new object();
        // Handle vers le panda
        private proto_hdle_t m_handle = null;

        //Device
        SWIGTYPE_p_proto_Device_t m_device = null;

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
        /// Ouverture de la connexion
        /// </summary>
        /// <param name="device"> device pour la connexion </param>
        /// <returns></returns>
        public int Open(SWIGTYPE_p_proto_Device_t device)
        {
            int ret = -1;

            m_device = device;

            //On ouvre la connexion
            m_handle = protocomm.proto_master_create(m_device); //5s de timeout TODO a moditifer plus tard pour le rendre paramètrable
            
            //On vérifie qu'il n'y a pas eu de problème lors de la connexion
            if(m_handle != null)
            {
                ret = 0;
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

            var uiValue = new_uint8_t_p();

            ret = protocomm.proto_master_get(m_handle, uiRegister, uiValue);

            //On récupère la valeur
            value = uint8_t_p_value(uiValue);

            return ret;
        }
    }
}
