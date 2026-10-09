using System;
using System.Drawing;
using System.Windows.Forms;

namespace CSharpUseC__AF_Dll
{
    public partial class Form1
    {
        private ComboBox _cbZOutputMode;
        private Label _lblZOutputMode;

        protected override void OnShown(EventArgs e)
        {
            base.OnShown(e);
            SetupZOutputModeControl();
        }

        private void SetupZOutputModeControl()
        {
            if (_cbZOutputMode != null)
                return;

            _lblZOutputMode = new Label
            {
                AutoSize = true,
                Location = new Point(20, 163),
                Text = "输出模式"
            };

            _cbZOutputMode = new ComboBox
            {
                DropDownStyle = ComboBoxStyle.DropDownList,
                Location = new Point(170, 158),
                Size = new Size(317, 28)
            };
            _cbZOutputMode.Items.AddRange(new object[]
            {
                "0 - 脉冲（SET_PUL，物理移动）",
                "1 - 回调（输出Z，不物理移动）",
                "2 - 模拟量（SET_AI，物理移动）"
            });

            // 默认使用脉冲输出；先赋值再绑定事件，避免窗口初始化时调用 DLL。
            _cbZOutputMode.SelectedIndex = 0;
            _cbZOutputMode.SelectedIndexChanged += ZOutputMode_SelectedIndexChanged;
            BT_Connect.Click += ApplyZOutputModeAfterConnect;

            GB_Axis.Controls.Add(_lblZOutputMode);
            GB_Axis.Controls.Add(_cbZOutputMode);
            _lblZOutputMode.BringToFront();
            _cbZOutputMode.BringToFront();
        }

        private void ZOutputMode_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (_connected)
                ApplyZOutputModeFromUi();
        }

        private void ApplyZOutputModeAfterConnect(object sender, EventArgs e)
        {
            if (_connected)
                ApplyZOutputModeFromUi();
        }

        internal AFNative.Result SetZOutputMode(int mode)
        {
            if (mode < 0 || mode > 2)
                throw new ArgumentOutOfRangeException("mode", "Z 输出模式必须是 0、1 或 2。");

            return AFNative.Set_Z_Output_Mode(mode);
        }

        private void ApplyZOutputModeFromUi()
        {
            int mode = _cbZOutputMode == null || _cbZOutputMode.SelectedIndex < 0
                ? 0
                : _cbZOutputMode.SelectedIndex;

            try
            {
                AFNative.Result result = SetZOutputMode(mode);
                AppendLog(result.IsSuccess
                    ? "Z 输出模式已切换为：" + _cbZOutputMode.Items[mode]
                    : "Z 输出模式切换失败：" + result.GetMessage());
            }
            catch (Exception ex)
            {
                AppendLog("Z 输出模式切换异常：" + ex.Message);
            }
        }
    }
}
