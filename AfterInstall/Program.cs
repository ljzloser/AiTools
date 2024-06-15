using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AfterInstall
{
    internal class Program
    {
        private static void Main(string[] args)
        {
            // 获取当前应用程序的目录
            string path = System.IO.Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().Location);
            // 执行命令
            System.Diagnostics.Process.Start(path + "\\AiTools.exe", "show");
        }
    }
}