//------------------------------------------------------------------------------
// <auto-generated />
//
// This file was automatically generated by SWIG (http://www.swig.org).
// Version 3.0.12
//
// Do not make changes to this file unless you know what you are doing--modify
// the SWIG interface file instead.
//------------------------------------------------------------------------------


public class proto_Frame_t : global::System.IDisposable {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal proto_Frame_t(global::System.IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(proto_Frame_t obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  ~proto_Frame_t() {
    Dispose();
  }

  public virtual void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          protocommPINVOKE.delete_proto_Frame_t(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }

  public byte startOfFrame {
    set {
      protocommPINVOKE.proto_Frame_t_startOfFrame_set(swigCPtr, value);
    } 
    get {
      byte ret = protocommPINVOKE.proto_Frame_t_startOfFrame_get(swigCPtr);
      return ret;
    } 
  }

  public byte crc8 {
    set {
      protocommPINVOKE.proto_Frame_t_crc8_set(swigCPtr, value);
    } 
    get {
      byte ret = protocommPINVOKE.proto_Frame_t_crc8_get(swigCPtr);
      return ret;
    } 
  }

  public byte command {
    set {
      protocommPINVOKE.proto_Frame_t_command_set(swigCPtr, value);
    } 
    get {
      byte ret = protocommPINVOKE.proto_Frame_t_command_get(swigCPtr);
      return ret;
    } 
  }

  public proto_frame_data_t data {
    set {
      protocommPINVOKE.proto_Frame_t_data_set(swigCPtr, proto_frame_data_t.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = protocommPINVOKE.proto_Frame_t_data_get(swigCPtr);
      proto_frame_data_t ret = (cPtr == global::System.IntPtr.Zero) ? null : new proto_frame_data_t(cPtr, false);
      return ret;
    } 
  }

  public proto_Frame_t() : this(protocommPINVOKE.new_proto_Frame_t(), true) {
  }

}
