using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Timers;

namespace SavingCurrentTimeService
{
    public class SavingCurrentTime
    {
        private readonly Timer timer;

        public SavingCurrentTime()
        {
            timer = new Timer(65000) { AutoReset = true };
            timer.Elapsed += TimerElapsed;
        }

        private void TimerElapsed(object sender, ElapsedEventArgs e)
        {
            string[] current_time = new string[] { DateTime.Now.ToString("h:mm:ss tt") };
            File.AppendAllLines(@"C:\SaveCurrentTime.txt", current_time);
        }

        public void Start()
        {
            timer.Start();
        }

        public void Stop()
        {
            timer.Stop();
        }
    }
}
