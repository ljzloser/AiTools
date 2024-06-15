using System;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Net.Http;
using System.Reflection;
using System.Threading.Tasks;
using System.Windows.Forms;
using Newtonsoft.Json.Linq;

namespace UpdateProgram
{
    internal class Program
    {
        [STAThread]
        private static void Main(string[] args)
        {
            // 获取第一个参数
            string param = args.Length > 0 ? args[0] : "";
            if (param != "")
            {
                double version;
                try
                {
                    version = double.Parse(param);
                }
                catch
                {
                    Console.WriteLine("param error, version must be a number");
                    return;
                }
                double newVersion = GetNewVersion();

                if (version < newVersion)
                {
                    DialogResult result = MessageBox.Show($"发现新版本(V{version}->V{newVersion})，是否更新？",
                        "AiTools更新提示", MessageBoxButtons.YesNo, MessageBoxIcon.Question);

                    if (result == DialogResult.Yes)
                    {
                        string appName = "AiTools";
                        KillApp(appName);
                        Application.EnableVisualStyles();
                        Application.SetCompatibleTextRenderingDefault(false);
                        Task.Run(async () => await CheckForUpdates(version)).Wait();
                    }
                }
                else
                {
                    if (args.Length > 1)
                    {
                        MessageBox.Show($"当前版本(V{version})为最新版本！", "AiTools更新提示",
                            MessageBoxButtons.OK, MessageBoxIcon.Information);
                    }
                }
            }
        }

        private static void KillApp(string appName)
        {
            Process[] processes = Process.GetProcessesByName(appName);
            if (processes.Length > 0)
            {
                foreach (Process process in processes)
                {
                    process.Kill();
                }
            }
        }

        private static double GetNewVersion()
        {
            string url = "https://api.github.com/repos/ljzloser/AiTools/releases/latest";

            // 配置 HttpClient
            HttpClient client = new HttpClient();
            client.DefaultRequestHeaders.UserAgent.TryParseAdd("request"); // 设置 User-Agent 头
                                                                           // 发送请求
            HttpResponseMessage response = client.GetAsync(url).Result;
            string result = response.Content.ReadAsStringAsync().Result;

            // 转换为json对象
            JObject obj = JObject.Parse(result);
            string tagName = obj["tag_name"]?.ToString().ToUpper();
            double versionNum = double.Parse(tagName.Replace("V", ""));
            return versionNum;
        }

        private static async Task CheckForUpdates(double version)
        {
            string path = Path.GetDirectoryName(Path.GetTempPath());
            string url = "https://api.github.com/repos/ljzloser/AiTools/releases/latest";

            // 配置 HttpClient
            HttpClient client = new HttpClient();
            client.DefaultRequestHeaders.UserAgent.TryParseAdd("request"); // 设置 User-Agent 头

            // 发送请求
            HttpResponseMessage response = await client.GetAsync(url);
            string result = await response.Content.ReadAsStringAsync();

            // 转换为json对象
            JObject obj = JObject.Parse(result);
            string tagName = obj["tag_name"]?.ToString().ToUpper();
            double versionNum = double.Parse(tagName.Replace("V", ""));

            if (versionNum > version)
            {
                JArray assets = (JArray)obj["assets"];
                if (assets.Count > 0)
                {
                    JObject asset = (JObject)assets[0];
                    string downloadUrl = asset["browser_download_url"]?.ToString();
                    string name = asset["name"]?.ToString();
                    string tempPath = Path.Combine(path, name);

                    if (File.Exists(tempPath))
                    {
                        File.Delete(tempPath);
                    }

                    var progressBar = new ProgressBarForm();

                    var taskCompletionSource = new TaskCompletionSource<bool>();

                    // 在UI线程中显示进度条窗口
                    Application.Run(new TaskSchedulerContext(progressBar, async () =>
                    {
                        await DownloadFileWithProgressAsync(client, downloadUrl, tempPath, progressBar);
                        taskCompletionSource.SetResult(true);  // 下载完成后设置任务完成
                    }));

                    await taskCompletionSource.Task;  // 等待任务完成
                    System.Diagnostics.Process.Start(tempPath);
                }
            }
        }

        private static async Task DownloadFileWithProgressAsync(HttpClient client, string downloadUrl, string destinationPath, ProgressBarForm progressBar)
        {
            using (HttpResponseMessage response = await client.GetAsync(downloadUrl, HttpCompletionOption.ResponseHeadersRead))
            {
                response.EnsureSuccessStatusCode();

                long? contentLength = response.Content.Headers.ContentLength;
                using (var contentStream = await response.Content.ReadAsStreamAsync())
                {
                    await ProcessContentStream(contentLength, contentStream, destinationPath, progressBar);
                }
            }
        }

        private static async Task ProcessContentStream(long? totalDownloadSize, Stream contentStream, string destinationPath, ProgressBarForm progressBar)
        {
            const int bufferSize = 81920;
            byte[] buffer = new byte[bufferSize];
            long totalBytesRead = 0;
            int bytesRead;

            using (FileStream fileStream = new FileStream(destinationPath, FileMode.Create, FileAccess.Write, FileShare.None, bufferSize, true))
            {
                while ((bytesRead = await contentStream.ReadAsync(buffer, 0, buffer.Length)) != 0)
                {
                    await fileStream.WriteAsync(buffer, 0, bytesRead);
                    totalBytesRead += bytesRead;

                    if (totalDownloadSize.HasValue)
                    {
                        int percentComplete = (int)(totalBytesRead * 100 / totalDownloadSize.Value);
                        if (progressBar.InvokeRequired)
                        {
                            progressBar.BeginInvoke(new Action<int>(progressBar.UpdateProgress), percentComplete);
                        }
                        else
                        {
                            progressBar.UpdateProgress(percentComplete);
                        }
                    }
                }
            }
        }
    }

    public class ProgressBarForm : Form
    {
        private ProgressBar progressBar;

        protected override void OnClosed(EventArgs e)
        {
            base.OnClosed(e);
            Application.Exit();
            Environment.Exit(0);
        }

        public ProgressBarForm()
        {
            this.Text = "Downloading AiTools...";
            this.Width = 400;
            this.Height = 100;
            this.Icon = Icon.FromHandle(Properties.Resources.Ai.GetHicon());
            this.StartPosition = FormStartPosition.CenterScreen;

            progressBar = new ProgressBar
            {
                Dock = DockStyle.Fill,
                Style = ProgressBarStyle.Continuous,
                Minimum = 0,
                Maximum = 100
            };

            this.Controls.Add(progressBar);
        }

        public void UpdateProgress(int percent)
        {
            if (percent >= 0 && percent <= 100)
            {
                progressBar.Value = percent;
                this.Text = $"Downloading... AiTools {percent}%";
            }
        }
    }

    public class TaskSchedulerContext : ApplicationContext
    {
        private readonly Task task;
        private readonly Form form;

        public TaskSchedulerContext(Form form, Func<Task> taskFunc)
        {
            this.form = form;
            this.task = taskFunc();
            this.task.ContinueWith(t => this.ExitThread(), TaskScheduler.FromCurrentSynchronizationContext());

            this.form.FormClosed += (sender, e) => this.ExitThread();
            this.form.Show();
        }
    }
}