
namespace IHM.Droid.Interfaces
{
    public class DroidLocalStoragePath : ILocalStoragePath
    {
        public string GetLocalStoragePath()
        {
            return System.Environment.GetFolderPath(System.Environment.SpecialFolder.Personal);
        }
    }
}