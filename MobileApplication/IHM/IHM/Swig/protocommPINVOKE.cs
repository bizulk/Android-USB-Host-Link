//------------------------------------------------------------------------------
// <auto-generated />
//
// This file was automatically generated by SWIG (http://www.swig.org).
// Version 3.0.12
//
// Do not make changes to this file unless you know what you are doing--modify
// the SWIG interface file instead.
//------------------------------------------------------------------------------


class protocommPINVOKE {

  protected class SWIGExceptionHelper {

    public delegate void ExceptionDelegate(string message);
    public delegate void ExceptionArgumentDelegate(string message, string paramName);

    static ExceptionDelegate applicationDelegate = new ExceptionDelegate(SetPendingApplicationException);
    static ExceptionDelegate arithmeticDelegate = new ExceptionDelegate(SetPendingArithmeticException);
    static ExceptionDelegate divideByZeroDelegate = new ExceptionDelegate(SetPendingDivideByZeroException);
    static ExceptionDelegate indexOutOfRangeDelegate = new ExceptionDelegate(SetPendingIndexOutOfRangeException);
    static ExceptionDelegate invalidCastDelegate = new ExceptionDelegate(SetPendingInvalidCastException);
    static ExceptionDelegate invalidOperationDelegate = new ExceptionDelegate(SetPendingInvalidOperationException);
    static ExceptionDelegate ioDelegate = new ExceptionDelegate(SetPendingIOException);
    static ExceptionDelegate nullReferenceDelegate = new ExceptionDelegate(SetPendingNullReferenceException);
    static ExceptionDelegate outOfMemoryDelegate = new ExceptionDelegate(SetPendingOutOfMemoryException);
    static ExceptionDelegate overflowDelegate = new ExceptionDelegate(SetPendingOverflowException);
    static ExceptionDelegate systemDelegate = new ExceptionDelegate(SetPendingSystemException);

    static ExceptionArgumentDelegate argumentDelegate = new ExceptionArgumentDelegate(SetPendingArgumentException);
    static ExceptionArgumentDelegate argumentNullDelegate = new ExceptionArgumentDelegate(SetPendingArgumentNullException);
    static ExceptionArgumentDelegate argumentOutOfRangeDelegate = new ExceptionArgumentDelegate(SetPendingArgumentOutOfRangeException);

    [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="SWIGRegisterExceptionCallbacks_protocomm")]
    public static extern void SWIGRegisterExceptionCallbacks_protocomm(
                                ExceptionDelegate applicationDelegate,
                                ExceptionDelegate arithmeticDelegate,
                                ExceptionDelegate divideByZeroDelegate, 
                                ExceptionDelegate indexOutOfRangeDelegate, 
                                ExceptionDelegate invalidCastDelegate,
                                ExceptionDelegate invalidOperationDelegate,
                                ExceptionDelegate ioDelegate,
                                ExceptionDelegate nullReferenceDelegate,
                                ExceptionDelegate outOfMemoryDelegate, 
                                ExceptionDelegate overflowDelegate, 
                                ExceptionDelegate systemExceptionDelegate);

    [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="SWIGRegisterExceptionArgumentCallbacks_protocomm")]
    public static extern void SWIGRegisterExceptionCallbacksArgument_protocomm(
                                ExceptionArgumentDelegate argumentDelegate,
                                ExceptionArgumentDelegate argumentNullDelegate,
                                ExceptionArgumentDelegate argumentOutOfRangeDelegate);

    static void SetPendingApplicationException(string message) {
      SWIGPendingException.Set(new global::System.ApplicationException(message, SWIGPendingException.Retrieve()));
    }
    static void SetPendingArithmeticException(string message) {
      SWIGPendingException.Set(new global::System.ArithmeticException(message, SWIGPendingException.Retrieve()));
    }
    static void SetPendingDivideByZeroException(string message) {
      SWIGPendingException.Set(new global::System.DivideByZeroException(message, SWIGPendingException.Retrieve()));
    }
    static void SetPendingIndexOutOfRangeException(string message) {
      SWIGPendingException.Set(new global::System.IndexOutOfRangeException(message, SWIGPendingException.Retrieve()));
    }
    static void SetPendingInvalidCastException(string message) {
      SWIGPendingException.Set(new global::System.InvalidCastException(message, SWIGPendingException.Retrieve()));
    }
    static void SetPendingInvalidOperationException(string message) {
      SWIGPendingException.Set(new global::System.InvalidOperationException(message, SWIGPendingException.Retrieve()));
    }
    static void SetPendingIOException(string message) {
      SWIGPendingException.Set(new global::System.IO.IOException(message, SWIGPendingException.Retrieve()));
    }
    static void SetPendingNullReferenceException(string message) {
      SWIGPendingException.Set(new global::System.NullReferenceException(message, SWIGPendingException.Retrieve()));
    }
    static void SetPendingOutOfMemoryException(string message) {
      SWIGPendingException.Set(new global::System.OutOfMemoryException(message, SWIGPendingException.Retrieve()));
    }
    static void SetPendingOverflowException(string message) {
      SWIGPendingException.Set(new global::System.OverflowException(message, SWIGPendingException.Retrieve()));
    }
    static void SetPendingSystemException(string message) {
      SWIGPendingException.Set(new global::System.SystemException(message, SWIGPendingException.Retrieve()));
    }

    static void SetPendingArgumentException(string message, string paramName) {
      SWIGPendingException.Set(new global::System.ArgumentException(message, paramName, SWIGPendingException.Retrieve()));
    }
    static void SetPendingArgumentNullException(string message, string paramName) {
      global::System.Exception e = SWIGPendingException.Retrieve();
      if (e != null) message = message + " Inner Exception: " + e.Message;
      SWIGPendingException.Set(new global::System.ArgumentNullException(paramName, message));
    }
    static void SetPendingArgumentOutOfRangeException(string message, string paramName) {
      global::System.Exception e = SWIGPendingException.Retrieve();
      if (e != null) message = message + " Inner Exception: " + e.Message;
      SWIGPendingException.Set(new global::System.ArgumentOutOfRangeException(paramName, message));
    }

    static SWIGExceptionHelper() {
      SWIGRegisterExceptionCallbacks_protocomm(
                                applicationDelegate,
                                arithmeticDelegate,
                                divideByZeroDelegate,
                                indexOutOfRangeDelegate,
                                invalidCastDelegate,
                                invalidOperationDelegate,
                                ioDelegate,
                                nullReferenceDelegate,
                                outOfMemoryDelegate,
                                overflowDelegate,
                                systemDelegate);

      SWIGRegisterExceptionCallbacksArgument_protocomm(
                                argumentDelegate,
                                argumentNullDelegate,
                                argumentOutOfRangeDelegate);
    }
  }

  protected static SWIGExceptionHelper swigExceptionHelper = new SWIGExceptionHelper();

  public class SWIGPendingException {
    [global::System.ThreadStatic]
    private static global::System.Exception pendingException = null;
    private static int numExceptionsPending = 0;

    public static bool Pending {
      get {
        bool pending = false;
        if (numExceptionsPending > 0)
          if (pendingException != null)
            pending = true;
        return pending;
      } 
    }

    public static void Set(global::System.Exception e) {
      if (pendingException != null)
        throw new global::System.ApplicationException("FATAL: An earlier pending exception from unmanaged code was missed and thus not thrown (" + pendingException.ToString() + ")", e);
      pendingException = e;
      lock(typeof(protocommPINVOKE)) {
        numExceptionsPending++;
      }
    }

    public static global::System.Exception Retrieve() {
      global::System.Exception e = null;
      if (numExceptionsPending > 0) {
        if (pendingException != null) {
          e = pendingException;
          pendingException = null;
          lock(typeof(protocommPINVOKE)) {
            numExceptionsPending--;
          }
        }
      }
      return e;
    }
  }


  protected class SWIGStringHelper {

    public delegate string SWIGStringDelegate(string message);
    static SWIGStringDelegate stringDelegate = new SWIGStringDelegate(CreateString);

    [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="SWIGRegisterStringCallback_protocomm")]
    public static extern void SWIGRegisterStringCallback_protocomm(SWIGStringDelegate stringDelegate);

    static string CreateString(string cString) {
      return cString;
    }

    static SWIGStringHelper() {
      SWIGRegisterStringCallback_protocomm(stringDelegate);
    }
  }

  static protected SWIGStringHelper swigStringHelper = new SWIGStringHelper();


  static protocommPINVOKE() {
  }


  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_new_uint8_t_p")]
  public static extern global::System.IntPtr new_uint8_t_p();

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_copy_uint8_t_p")]
  public static extern global::System.IntPtr copy_uint8_t_p(byte jarg1);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_delete_uint8_t_p")]
  public static extern void delete_uint8_t_p(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_uint8_t_p_assign")]
  public static extern void uint8_t_p_assign(global::System.Runtime.InteropServices.HandleRef jarg1, byte jarg2);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_uint8_t_p_value")]
  public static extern byte uint8_t_p_value(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_new_proto_Command_t_p")]
  public static extern global::System.IntPtr new_proto_Command_t_p();

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_copy_proto_Command_t_p")]
  public static extern global::System.IntPtr copy_proto_Command_t_p(int jarg1);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_delete_proto_Command_t_p")]
  public static extern void delete_proto_Command_t_p(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_proto_Command_t_p_assign")]
  public static extern void proto_Command_t_p_assign(global::System.Runtime.InteropServices.HandleRef jarg1, int jarg2);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_proto_Command_t_p_value")]
  public static extern int proto_Command_t_p_value(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_PROTO_FRAME_RECV_TOUT_MS_get")]
  public static extern int PROTO_FRAME_RECV_TOUT_MS_get();

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_proto_cio_open")]
  public static extern global::System.IntPtr proto_cio_open(string jarg1);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_proto_master_create")]
  public static extern global::System.IntPtr proto_master_create(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_proto_master_destroy")]
  public static extern void proto_master_destroy(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_proto_master_open")]
  public static extern int proto_master_open(global::System.Runtime.InteropServices.HandleRef jarg1, string jarg2);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_proto_master_close")]
  public static extern int proto_master_close(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_proto_master_get")]
  public static extern int proto_master_get(global::System.Runtime.InteropServices.HandleRef jarg1, byte jarg2, global::System.Runtime.InteropServices.HandleRef jarg3);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_proto_master_set")]
  public static extern int proto_master_set(global::System.Runtime.InteropServices.HandleRef jarg1, byte jarg2, byte jarg3);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_proto_frame_data_t_raw_set")]
  public static extern void proto_frame_data_t_raw_set(global::System.Runtime.InteropServices.HandleRef jarg1, global::System.Runtime.InteropServices.HandleRef jarg2);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_proto_frame_data_t_raw_get")]
  public static extern global::System.IntPtr proto_frame_data_t_raw_get(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_proto_frame_data_t_req_get")]
  public static extern global::System.IntPtr proto_frame_data_t_req_get(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_proto_frame_data_t_reg_value_set")]
  public static extern void proto_frame_data_t_reg_value_set(global::System.Runtime.InteropServices.HandleRef jarg1, byte jarg2);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_proto_frame_data_t_reg_value_get")]
  public static extern byte proto_frame_data_t_reg_value_get(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_proto_frame_data_t_crcerr_set")]
  public static extern void proto_frame_data_t_crcerr_set(global::System.Runtime.InteropServices.HandleRef jarg1, global::System.Runtime.InteropServices.HandleRef jarg2);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_proto_frame_data_t_crcerr_get")]
  public static extern global::System.IntPtr proto_frame_data_t_crcerr_get(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_new_proto_frame_data_t")]
  public static extern global::System.IntPtr new_proto_frame_data_t();

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_delete_proto_frame_data_t")]
  public static extern void delete_proto_frame_data_t(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_proto_frame_data_req_reg_set")]
  public static extern void proto_frame_data_req_reg_set(global::System.Runtime.InteropServices.HandleRef jarg1, byte jarg2);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_proto_frame_data_req_reg_get")]
  public static extern byte proto_frame_data_req_reg_get(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_proto_frame_data_req_value_set")]
  public static extern void proto_frame_data_req_value_set(global::System.Runtime.InteropServices.HandleRef jarg1, byte jarg2);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_proto_frame_data_req_value_get")]
  public static extern byte proto_frame_data_req_value_get(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_new_proto_frame_data_req")]
  public static extern global::System.IntPtr new_proto_frame_data_req();

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_delete_proto_frame_data_req")]
  public static extern void delete_proto_frame_data_req(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_proto_Frame_t_startOfFrame_set")]
  public static extern void proto_Frame_t_startOfFrame_set(global::System.Runtime.InteropServices.HandleRef jarg1, byte jarg2);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_proto_Frame_t_startOfFrame_get")]
  public static extern byte proto_Frame_t_startOfFrame_get(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_proto_Frame_t_crc8_set")]
  public static extern void proto_Frame_t_crc8_set(global::System.Runtime.InteropServices.HandleRef jarg1, byte jarg2);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_proto_Frame_t_crc8_get")]
  public static extern byte proto_Frame_t_crc8_get(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_proto_Frame_t_command_set")]
  public static extern void proto_Frame_t_command_set(global::System.Runtime.InteropServices.HandleRef jarg1, byte jarg2);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_proto_Frame_t_command_get")]
  public static extern byte proto_Frame_t_command_get(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_proto_Frame_t_data_set")]
  public static extern void proto_Frame_t_data_set(global::System.Runtime.InteropServices.HandleRef jarg1, global::System.Runtime.InteropServices.HandleRef jarg2);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_proto_Frame_t_data_get")]
  public static extern global::System.IntPtr proto_Frame_t_data_get(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_new_proto_Frame_t")]
  public static extern global::System.IntPtr new_proto_Frame_t();

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_delete_proto_Frame_t")]
  public static extern void delete_proto_Frame_t(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_proto_START_OF_FRAME_get")]
  public static extern int proto_START_OF_FRAME_get();

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_proto_COMMAND_OFFSET_get")]
  public static extern int proto_COMMAND_OFFSET_get();

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_proto_ARGS_OFFSET_get")]
  public static extern int proto_ARGS_OFFSET_get();

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_proto_FRAME_MAXSIZE_get")]
  public static extern int proto_FRAME_MAXSIZE_get();

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_proto_hdle_t_priv_callback_set")]
  public static extern void proto_hdle_t_priv_callback_set(global::System.Runtime.InteropServices.HandleRef jarg1, global::System.Runtime.InteropServices.HandleRef jarg2);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_proto_hdle_t_priv_callback_get")]
  public static extern global::System.IntPtr proto_hdle_t_priv_callback_get(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_proto_hdle_t_priv_userdata_set")]
  public static extern void proto_hdle_t_priv_userdata_set(global::System.Runtime.InteropServices.HandleRef jarg1, global::System.Runtime.InteropServices.HandleRef jarg2);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_proto_hdle_t_priv_userdata_get")]
  public static extern global::System.IntPtr proto_hdle_t_priv_userdata_get(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_proto_hdle_t_priv_frame_set")]
  public static extern void proto_hdle_t_priv_frame_set(global::System.Runtime.InteropServices.HandleRef jarg1, global::System.Runtime.InteropServices.HandleRef jarg2);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_proto_hdle_t_priv_frame_get")]
  public static extern global::System.IntPtr proto_hdle_t_priv_frame_get(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_proto_hdle_t_priv_nbBytes_set")]
  public static extern void proto_hdle_t_priv_nbBytes_set(global::System.Runtime.InteropServices.HandleRef jarg1, byte jarg2);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_proto_hdle_t_priv_nbBytes_get")]
  public static extern byte proto_hdle_t_priv_nbBytes_get(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_proto_hdle_t_priv_iodevice_set")]
  public static extern void proto_hdle_t_priv_iodevice_set(global::System.Runtime.InteropServices.HandleRef jarg1, global::System.Runtime.InteropServices.HandleRef jarg2);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_proto_hdle_t_priv_iodevice_get")]
  public static extern global::System.IntPtr proto_hdle_t_priv_iodevice_get(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_new_proto_hdle_t")]
  public static extern global::System.IntPtr new_proto_hdle_t();

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_delete_proto_hdle_t")]
  public static extern void delete_proto_hdle_t(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_proto_create")]
  public static extern global::System.IntPtr proto_create(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_proto_init")]
  public static extern void proto_init(global::System.Runtime.InteropServices.HandleRef jarg1, global::System.Runtime.InteropServices.HandleRef jarg2);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_proto_destroy")]
  public static extern void proto_destroy(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_proto_open")]
  public static extern int proto_open(global::System.Runtime.InteropServices.HandleRef jarg1, string jarg2);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_proto_close")]
  public static extern int proto_close(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_PROTO_WAIT_FOREVER_get")]
  public static extern int PROTO_WAIT_FOREVER_get();

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_proto_readFrame")]
  public static extern int proto_readFrame(global::System.Runtime.InteropServices.HandleRef jarg1, short jarg2);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_proto_decodeFrame")]
  public static extern int proto_decodeFrame(global::System.Runtime.InteropServices.HandleRef jarg1, global::System.Runtime.InteropServices.HandleRef jarg2, global::System.Runtime.InteropServices.HandleRef jarg3);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_proto_writeFrame")]
  public static extern int proto_writeFrame(global::System.Runtime.InteropServices.HandleRef jarg1, int jarg2, global::System.Runtime.InteropServices.HandleRef jarg3);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_proto_getArgsSize")]
  public static extern byte proto_getArgsSize(int jarg1);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_proto_setReceiver")]
  public static extern void proto_setReceiver(global::System.Runtime.InteropServices.HandleRef jarg1, global::System.Runtime.InteropServices.HandleRef jarg2, global::System.Runtime.InteropServices.HandleRef jarg3);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_proto_pushToFrame")]
  public static extern int proto_pushToFrame(global::System.Runtime.InteropServices.HandleRef jarg1, [global::System.Runtime.InteropServices.In, global::System.Runtime.InteropServices.MarshalAs(global::System.Runtime.InteropServices.UnmanagedType.LPArray)]byte[] jarg2, uint jarg3);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_proto_makeFrame")]
  public static extern byte proto_makeFrame(global::System.Runtime.InteropServices.HandleRef jarg1, int jarg2, global::System.Runtime.InteropServices.HandleRef jarg3);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_EMULSLAVE_NB_REGS_get")]
  public static extern int EMULSLAVE_NB_REGS_get();

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_devemulslave_create")]
  public static extern global::System.IntPtr devemulslave_create();

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_devemulslave_getRegisters")]
  public static extern global::System.IntPtr devemulslave_getRegisters(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_devemulslave_setFlags")]
  public static extern void devemulslave_setFlags(global::System.Runtime.InteropServices.HandleRef jarg1, byte jarg2);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_devemulslave_getFlags")]
  public static extern void devemulslave_getFlags(global::System.Runtime.InteropServices.HandleRef jarg1, global::System.Runtime.InteropServices.HandleRef jarg2);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_devserial_create")]
  public static extern global::System.IntPtr devserial_create();

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_devserial_setFD")]
  public static extern int devserial_setFD(global::System.Runtime.InteropServices.HandleRef jarg1, int jarg2);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_devserial_getFD")]
  public static extern int devserial_getFD(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_devusbdev_create")]
  public static extern global::System.IntPtr devusbdev_create();

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_devusbdev_setDev")]
  public static extern int devusbdev_setDev(global::System.Runtime.InteropServices.HandleRef jarg1, int jarg2, int jarg3, int jarg4, int jarg5);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_devusbdev_getFD")]
  public static extern int devusbdev_getFD(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_devlibusb_create")]
  public static extern global::System.IntPtr devlibusb_create();

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_devlibusb_setFD")]
  public static extern int devlibusb_setFD(global::System.Runtime.InteropServices.HandleRef jarg1, int jarg2, int jarg3, int jarg4, int jarg5);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_devlibusb_getFD")]
  public static extern int devlibusb_getFD(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_LOG_MSG_LEN_get")]
  public static extern int LOG_MSG_LEN_get();

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_msg_t_szMsg_set")]
  public static extern void msg_t_szMsg_set(global::System.Runtime.InteropServices.HandleRef jarg1, string jarg2);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_msg_t_szMsg_get")]
  public static extern string msg_t_szMsg_get(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_new_msg_t")]
  public static extern global::System.IntPtr new_msg_t();

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_delete_msg_t")]
  public static extern void delete_msg_t(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_log_create")]
  public static extern global::System.IntPtr log_create(uint jarg1);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_log_destroy")]
  public static extern int log_destroy(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_log_push")]
  public static extern int log_push(global::System.Runtime.InteropServices.HandleRef jarg1, string jarg2);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_log_pop")]
  public static extern int log_pop(global::System.Runtime.InteropServices.HandleRef jarg1, string jarg2);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_log_getsize")]
  public static extern uint log_getsize(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_log_getfree")]
  public static extern uint log_getfree(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_log_clear")]
  public static extern void log_clear(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_log_global_create")]
  public static extern int log_global_create(uint jarg1);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_log_global_destroy")]
  public static extern int log_global_destroy();

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_log_global_push")]
  public static extern int log_global_push(string jarg1);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_log_global_pop")]
  public static extern int log_global_pop(string jarg1);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_log_global_pop_msg")]
  public static extern int log_global_pop_msg(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_log_global_getsize")]
  public static extern uint log_global_getsize();

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_log_global_getfree")]
  public static extern uint log_global_getfree();

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_log_global_clear")]
  public static extern void log_global_clear();

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_protoframe_serialize")]
  public static extern void protoframe_serialize(global::System.Runtime.InteropServices.HandleRef jarg1, [global::System.Runtime.InteropServices.Out, global::System.Runtime.InteropServices.MarshalAs(global::System.Runtime.InteropServices.UnmanagedType.LPArray)]byte[] jarg2);

  [global::System.Runtime.InteropServices.DllImport("protocomm", EntryPoint="CSharp_sizeof_proto_Frame_t")]
  public static extern int sizeof_proto_Frame_t();
}
