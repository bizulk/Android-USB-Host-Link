using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
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
        /// Fonctions retournant une liste du nom de chaque connections 
        /// </summary>
        /// <returns></returns>
        ICollection<string> getListOfConnections();

        /// <summary>
        /// Fonction de désallocation des ressources
        /// </summary>
        /// <returns></returns>
        int Close();
    }
}
