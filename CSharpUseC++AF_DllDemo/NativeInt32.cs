using System;
using System.Runtime.InteropServices;

namespace CSharpUseC__AF_Dll
{
    /// <summary>
    /// 长期持有一块 4 字节非托管内存，用于传给 C++ 的 int*。
    /// 在 DLL 停止使用该指针之前不能释放。
    /// </summary>
    internal sealed class NativeInt32 : IDisposable
    {
        private IntPtr _pointer;

        public NativeInt32(int initialValue)
        {
            _pointer = Marshal.AllocHGlobal(sizeof(int));
            Marshal.WriteInt32(_pointer, initialValue);
        }

        public IntPtr Pointer
        {
            get
            {
                if (_pointer == IntPtr.Zero)
                    throw new ObjectDisposedException(nameof(NativeInt32));

                return _pointer;
            }
        }

        public int Value
        {
            get { return Marshal.ReadInt32(Pointer); }
            set { Marshal.WriteInt32(Pointer, value); }
        }

        public void Dispose()
        {
            if (_pointer == IntPtr.Zero)
                return;

            Marshal.FreeHGlobal(_pointer);
            _pointer = IntPtr.Zero;
            GC.SuppressFinalize(this);
        }

        ~NativeInt32()
        {
            if (_pointer != IntPtr.Zero)
            {
                Marshal.FreeHGlobal(_pointer);
                _pointer = IntPtr.Zero;
            }
        }
    }
}
