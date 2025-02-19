using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.Foundation;
using Windows.Management.Deployment;
using System.Runtime.InteropServices;
using System.Threading;

namespace PackageAdd
{
    public enum AsyncStatus
    {
        Started,
        Completed,
        Canceled,
        Error
    }
    public delegate void PACKAGE_ADD_CALLBACK (int progress);
    public static class PackageInstaller
    {
        public static AsyncStatus AddPackageFromFile([MarshalAs(UnmanagedType.LPWStr)] string swFilePath, IntPtr callbackPtr)
        {
            if (string.IsNullOrEmpty(swFilePath))
            {
                return AsyncStatus.Error;
            }
            try
            {
                Uri packageUri = new Uri(swFilePath);
                PackageManager packageManager = new PackageManager();
                IAsyncOperationWithProgress<DeploymentResult, DeploymentProgress> deploymentOperation = packageManager.AddPackageAsync(packageUri, null, DeploymentOptions.None);
                // 获取 C++ 传入的回调函数
                PACKAGE_ADD_CALLBACK callback = (PACKAGE_ADD_CALLBACK)Marshal.GetDelegateForFunctionPointer(callbackPtr, typeof(PACKAGE_ADD_CALLBACK));
                ManualResetEvent opCompletedEvent = new ManualResetEvent(false);
                deploymentOperation.Progress = (depProgress, progress) =>
                {
                    callback?.Invoke((int)progress.percentage);
                };
                deploymentOperation.Completed = (depProgress, status) =>
                {
                    opCompletedEvent.Set();
                };
                //Console.WriteLine($"Installing package {swFilePath}");
                opCompletedEvent.WaitOne();
                if (deploymentOperation.Status == Windows.Foundation.AsyncStatus.Error)
                {
                    return AsyncStatus.Error;
                }
                else if (deploymentOperation.Status == Windows.Foundation.AsyncStatus.Canceled)
                {
                    return AsyncStatus.Canceled;
                }
                else if (deploymentOperation.Status == Windows.Foundation.AsyncStatus.Completed)
                {
                    return AsyncStatus.Completed;
                }
                else
                {
                    return AsyncStatus.Error;
                }
            }
            catch (Exception)
            {
                return AsyncStatus.Error;
            }
        }
    }
}
