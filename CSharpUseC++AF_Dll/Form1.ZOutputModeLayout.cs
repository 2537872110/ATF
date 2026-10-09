using System;
using System.Drawing;

namespace CSharpUseC__AF_Dll
{
    public partial class Form1
    {
        protected override void OnLoad(EventArgs e)
        {
            base.OnLoad(e);
            SetupZOutputModeControl();
            LayoutZOutputModeControl();
        }

        protected override void OnLayout(System.Windows.Forms.LayoutEventArgs levent)
        {
            base.OnLayout(levent);

            LayoutZOutputModeControl();
        }

        private void LayoutZOutputModeControl()
        {

            if (_lblZOutputMode == null || _cbZOutputMode == null)
                return;

            // Use a dedicated row below STOP/Home so the selector never
            // overlaps the existing Z-axis controls at high DPI.
            int rowTop = Math.Max(246, GB_Axis.ClientSize.Height - 36);
            _lblZOutputMode.Location = new Point(20, rowTop + 5);
            _cbZOutputMode.Location = new Point(170, rowTop);
            _cbZOutputMode.Width = Math.Max(180, GB_Axis.ClientSize.Width - 201);
        }
    }
}
