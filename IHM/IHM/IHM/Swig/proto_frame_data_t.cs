//------------------------------------------------------------------------------
// <auto-generated />
//
// This file was automatically generated by SWIG (http://www.swig.org).
// Version 3.0.12
//
// Do not make changes to this file unless you know what you are doing--modify
// the SWIG interface file instead.
//------------------------------------------------------------------------------


public class proto_frame_data_t : global::System.IDisposable {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal proto_frame_data_t(global::System.IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(proto_frame_data_t obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  ~proto_frame_data_t() {
    Dispose();
  }

  public virtual void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          protocommPINVOKE.delete_proto_frame_data_t(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }

  public SWIGTYPE_p_unsigned_char raw {
    set {
      protocommPINVOKE.proto_frame_data_t_raw_set(swigCPtr, SWIGTYPE_p_unsigned_char.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = protocommPINVOKE.proto_frame_data_t_raw_get(swigCPtr);
      SWIGTYPE_p_unsigned_char ret = (cPtr == global::System.IntPtr.Zero) ? null : new SWIGTYPE_p_unsigned_char(cPtr, false);
      return ret;
    } 
  }

  public proto_frame_data_req req {
    get {
      global::System.IntPtr cPtr = protocommPINVOKE.proto_frame_data_t_req_get(swigCPtr);
      proto_frame_data_req ret = (cPtr == global::System.IntPtr.Zero) ? null : new proto_frame_data_req(cPtr, false);
      return ret;
    } 
  }

  public byte reg_value {
    set {
      protocommPINVOKE.proto_frame_data_t_reg_value_set(swigCPtr, value);
    } 
    get {
      byte ret = protocommPINVOKE.proto_frame_data_t_reg_value_get(swigCPtr);
      return ret;
    } 
  }

  public SWIGTYPE_p_unsigned_char crcerr {
    set {
      protocommPINVOKE.proto_frame_data_t_crcerr_set(swigCPtr, SWIGTYPE_p_unsigned_char.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = protocommPINVOKE.proto_frame_data_t_crcerr_get(swigCPtr);
      SWIGTYPE_p_unsigned_char ret = (cPtr == global::System.IntPtr.Zero) ? null : new SWIGTYPE_p_unsigned_char(cPtr, false);
      return ret;
    } 
  }

  public proto_frame_data_t() : this(protocommPINVOKE.new_proto_frame_data_t(), true) {
  }

}
