using System;
using System.Drawing;
using System.Windows.Forms;

namespace CSharpUseC__AF_Dll
{
    public partial class Form1
    {
        private const int ZOutputModeWidth = 632;
        private const int ZOutputModeHeight = 40;
        private const int ZOutputModeTopSpacing = 6;
        private ZOutputModeSelector _designerZOutputMode;

        protected override void OnShown(EventArgs e)
        {
            base.OnShown(e);
            if (_designerZOutputMode != null) return;

            GroupBox legacyGroup = null;
            foreach (Control control in Controls)
            {
                GroupBox group = control as GroupBox;
                if (group != null && group.Text == "Z 输出模式") { legacyGroup = group; break; }
            }

            Point location = legacyGroup == null
                ? new Point(GB_ObjectiveManagement.Left, GB_ObjectiveManagement.Bottom + ZOutputModeTopSpacing)
                : legacyGroup.Location;
            if (legacyGroup != null) legacyGroup.Visible = false;

            _designerZOutputMode = new ZOutputModeSelector
            {
                Name = "ZOutputModeSelector",
                Location = location,
                Size = new Size(ZOutputModeWidth, ZOutputModeHeight),
                Anchor = AnchorStyles.Left | AnchorStyles.Right | AnchorStyles.Bottom,
                SelectedMode = 0
            };
            _designerZOutputMode.SelectedModeChanged += DesignerZOutputMode_Changed;
            Controls.Add(_designerZOutputMode);
            _designerZOutputMode.BringToFront();
        }

        private void DesignerZOutputMode_Changed(object sender, EventArgs e)
        {
            if (_cbZOutputMode != null) _cbZOutputMode.SelectedIndex = _designerZOutputMode.SelectedMode;
        }
    }
}
