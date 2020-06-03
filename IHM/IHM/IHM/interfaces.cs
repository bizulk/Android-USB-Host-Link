using System;
using System.Collections.Generic;
using System.Text;

namespace IHM
{
    /// <summary>
    /// Cette classe définit l'API d'interface
    /// </summary>
    public interface IUsbManager
    {
        /// <summary>
        /// Fonction initialisant la ressource au niveau système
        /// </summary>
        /// <returns> pourquoi pas le FD, pour l'instant je fais comme ça</returns>
        int Init();

        /// <summary>
        /// Fonction de désallocation des ressources
        /// </summary>
        /// <returns></returns>
        int Close();
    }
}
