//------------------------------------------------------------------------------
// <auto-generated />
//
// This file was automatically generated by SWIG (http://www.swig.org).
// Version 3.0.12
//
// Do not make changes to this file unless you know what you are doing--modify
// the SWIG interface file instead.
//------------------------------------------------------------------------------


public class proto_IfaceIODevice_t : global::System.IDisposable {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal proto_IfaceIODevice_t(global::System.IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(proto_IfaceIODevice_t obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  ~proto_IfaceIODevice_t() {
    Dispose();
  }

  public virtual void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          protocommPINVOKE.delete_proto_IfaceIODevice_t(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }

  public SWIGTYPE_p_f_p_struct_proto_IfaceIODevice_p_q_const__char__int open {
    set {
      protocommPINVOKE.proto_IfaceIODevice_t_open_set(swigCPtr, SWIGTYPE_p_f_p_struct_proto_IfaceIODevice_p_q_const__char__int.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = protocommPINVOKE.proto_IfaceIODevice_t_open_get(swigCPtr);
      SWIGTYPE_p_f_p_struct_proto_IfaceIODevice_p_q_const__char__int ret = (cPtr == global::System.IntPtr.Zero) ? null : new SWIGTYPE_p_f_p_struct_proto_IfaceIODevice_p_q_const__char__int(cPtr, false);
      return ret;
    } 
  }

  public SWIGTYPE_p_f_p_struct_proto_IfaceIODevice__int close {
    set {
      protocommPINVOKE.proto_IfaceIODevice_t_close_set(swigCPtr, SWIGTYPE_p_f_p_struct_proto_IfaceIODevice__int.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = protocommPINVOKE.proto_IfaceIODevice_t_close_get(swigCPtr);
      SWIGTYPE_p_f_p_struct_proto_IfaceIODevice__int ret = (cPtr == global::System.IntPtr.Zero) ? null : new SWIGTYPE_p_f_p_struct_proto_IfaceIODevice__int(cPtr, false);
      return ret;
    } 
  }

  public SWIGTYPE_p_f_p_struct_proto_IfaceIODevice__void destroy {
    set {
      protocommPINVOKE.proto_IfaceIODevice_t_destroy_set(swigCPtr, SWIGTYPE_p_f_p_struct_proto_IfaceIODevice__void.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = protocommPINVOKE.proto_IfaceIODevice_t_destroy_get(swigCPtr);
      SWIGTYPE_p_f_p_struct_proto_IfaceIODevice__void ret = (cPtr == global::System.IntPtr.Zero) ? null : new SWIGTYPE_p_f_p_struct_proto_IfaceIODevice__void(cPtr, false);
      return ret;
    } 
  }

  public SWIGTYPE_p_f_p_struct_proto_IfaceIODevice_p_void_unsigned_char_short__int read {
    set {
      protocommPINVOKE.proto_IfaceIODevice_t_read_set(swigCPtr, SWIGTYPE_p_f_p_struct_proto_IfaceIODevice_p_void_unsigned_char_short__int.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = protocommPINVOKE.proto_IfaceIODevice_t_read_get(swigCPtr);
      SWIGTYPE_p_f_p_struct_proto_IfaceIODevice_p_void_unsigned_char_short__int ret = (cPtr == global::System.IntPtr.Zero) ? null : new SWIGTYPE_p_f_p_struct_proto_IfaceIODevice_p_void_unsigned_char_short__int(cPtr, false);
      return ret;
    } 
  }

  public SWIGTYPE_p_f_p_struct_proto_IfaceIODevice_p_q_const__void_unsigned_char__int write {
    set {
      protocommPINVOKE.proto_IfaceIODevice_t_write_set(swigCPtr, SWIGTYPE_p_f_p_struct_proto_IfaceIODevice_p_q_const__void_unsigned_char__int.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = protocommPINVOKE.proto_IfaceIODevice_t_write_get(swigCPtr);
      SWIGTYPE_p_f_p_struct_proto_IfaceIODevice_p_q_const__void_unsigned_char__int ret = (cPtr == global::System.IntPtr.Zero) ? null : new SWIGTYPE_p_f_p_struct_proto_IfaceIODevice_p_q_const__void_unsigned_char__int(cPtr, false);
      return ret;
    } 
  }

  public SWIGTYPE_p_void user {
    set {
      protocommPINVOKE.proto_IfaceIODevice_t_user_set(swigCPtr, SWIGTYPE_p_void.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = protocommPINVOKE.proto_IfaceIODevice_t_user_get(swigCPtr);
      SWIGTYPE_p_void ret = (cPtr == global::System.IntPtr.Zero) ? null : new SWIGTYPE_p_void(cPtr, false);
      return ret;
    } 
  }

  public proto_IfaceIODevice_t() : this(protocommPINVOKE.new_proto_IfaceIODevice_t(), true) {
  }

}