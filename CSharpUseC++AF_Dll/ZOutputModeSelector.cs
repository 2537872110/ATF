using System;
using System.Windows.Forms;

namespace CSharpUseC__AF_Dll
{
    public partial class ZOutputModeSelector : UserControl
    {
        public event EventHandler SelectedModeChanged;

        public ZOutputModeSelector()
        {
            InitializeComponent();
            modeCombo.SelectedIndex = 0;
        }

        public int SelectedMode
        {
            get { return modeCombo.SelectedIndex < 0 ? 0 : modeCombo.SelectedIndex; }
            set { modeCombo.SelectedIndex = Math.Max(0, Math.Min(2, value)); }
        }

        private void modeCombo_SelectedIndexChanged(object sender, EventArgs e)
        {
            SelectedModeChanged?.Invoke(this, EventArgs.Empty);
        }
    }
}
