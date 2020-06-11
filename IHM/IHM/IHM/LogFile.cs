using Plugin.XFLogger;
using System;

namespace IHM
{
    public sealed class LogFile
    {
        private static LogFile _instance = null;

        private LogFile()
        {
        }

        static internal LogFile Instance()
        {
            if (_instance == null)
            {
                _instance = new LogFile();
            }

            return _instance;
        }

        public void Info(string message, string tag = "")
        {
            CrossXFLogger.Current.Info(message, tag);
        }

        public void Debug(string message, string tag = "")
        {
            CrossXFLogger.Current.Debug(message, tag);
        }

        public void Error(string message, string tag = "")
        {
            CrossXFLogger.Current.Error(message, tag);
        }

        public void Error(Exception ex, string message, string tag = "")
        {
            CrossXFLogger.Current.Error(tag, message, ex);
        }

        public void Warn(string message, string tag = "")
        {
            CrossXFLogger.Current.Warn(message, tag);
        }

        public void Clear()
        {
            CrossXFLogger.Current.Purge();
        }

        public string GetLog(bool requireReverse = true)
        {
            return CrossXFLogger.Current.GetAll(requireReverse);
        }

        public string GetLocalStoragePath()
        {
            return CrossXFLogger.Current.GetLocalStoragePath();
        }
    }
}
