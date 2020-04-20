using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Topshelf;

namespace SavingCurrentTimeService
{
    class Program
    {
        static void Main(string[] args)
        {
            var exitCode = HostFactory.Run(x =>
            {
                x.Service<SavingCurrentTime>(s =>
                {
                    s.ConstructUsing(savingCurrentTime => new SavingCurrentTime());
                    s.WhenStarted(savingCurrentTime => savingCurrentTime.Start());
                    s.WhenStopped(savingCurrentTime => savingCurrentTime.Stop());
                });

                x.RunAsLocalSystem();

                x.SetServiceName("SavingCurrentTime");
                x.SetDisplayName("Saving Current Time");
                x.SetDescription("This is a service program which writes the current system time in a text file.");
            });

            int exitCodeValue = (int)Convert.ChangeType(exitCode, exitCode.GetTypeCode());
            Environment.ExitCode = exitCodeValue;
        }
    }
}
