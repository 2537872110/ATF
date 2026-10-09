using System;
using System.Runtime.InteropServices;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace CSharpUseC__AF_Dll
{
    internal partial class FeatureConsoleForm
    {
        private bool _graphClosePending;
        private bool _graphDelayEventsAttached;

        [DllImport("AF.dll", CallingConvention = CallingConvention.Cdecl,
            ExactSpelling = true)]
        private static extern void Configure_Graph_Scan_Delay(
            int start,
            int end);

        protected override void OnShown(EventArgs e)
        {
            base.OnShown(e);
            if (_graphDelayEventsAttached)
                return;
            _graphDelayEventsAttached = true;
            _graphRun.MouseDown += ConfigureGraphEndpointDelay;
            _graphRunSeven.MouseDown += ConfigureGraphEndpointDelay;
            _graphRun.Click += RestoreMainUiAfterGraphRun;
            _graphRunSeven.Click += RestoreMainUiAfterGraphRun;
            _graphStop.Click += RestoreMainUiAfterGraphStop;
        }

        private async void RestoreMainUiAfterGraphRun(object sender, EventArgs e)
        {
            // The original Run handler creates the cancellation source before
            // its first await.  Observe that same lifetime so normal completion
            // and cancellation both use the identical UI recovery path.
            if (_graphCancellation == null)
                return;

            while (!IsDisposed && _graphCancellation != null)
                await Task.Delay(25);

            if (!IsDisposed && !_main.IsDisposed)
                _main.ForceEndFeatureOperationUi();
        }

        private async void RestoreMainUiAfterGraphStop(object sender, EventArgs e)
        {
            // GraphStop_Click only requests cancellation.  Wait for the scan
            // worker's finally block to finish moving/disposing before the
            // main form is made interactive again.
            while (!IsDisposed && _graphCancellation != null)
                await Task.Delay(25);

            if (!IsDisposed && !_main.IsDisposed)
                _main.ForceEndFeatureOperationUi();
        }

        private void ConfigureGraphEndpointDelay(object sender, MouseEventArgs e)
        {
            Configure_Graph_Scan_Delay(
                Decimal.ToInt32(Decimal.Round(_graphStart.Value, 0)),
                Decimal.ToInt32(Decimal.Round(_graphEnd.Value, 0)));
        }

        protected override void OnFormClosing(FormClosingEventArgs e)
        {
            if (e.CloseReason == CloseReason.UserClosing &&
                !_main.IsDisposed &&
                _graphCancellation != null)
            {
                e.Cancel = true;
                _graphCancellation.Cancel();
                if (!_graphClosePending)
                {
                    _graphClosePending = true;
                    FinishGraphCloseAsync();
                }
                return;
            }

            base.OnFormClosing(e);
        }

        private async void FinishGraphCloseAsync()
        {
            try
            {
                // The scan finally block returns Z and releases the operation
                // lock. Hide only after that cleanup has actually completed.
                while (!IsDisposed && _graphCancellation != null)
                    await Task.Delay(25);

                if (!IsDisposed)
                {
                    _main.ForceEndFeatureOperationUi();
                    Hide();
                }
            }
            finally
            {
                _graphClosePending = false;
            }
        }
    }
}
