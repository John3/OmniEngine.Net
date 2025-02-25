


#region
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using WinterLeaf.Engine;
using WinterLeaf.Engine.Classes;
using WinterLeaf.Engine.Containers;
using WinterLeaf.Engine.Enums;
using System.ComponentModel;
using System.Threading;
using  WinterLeaf.Engine.Classes.Interopt;
using WinterLeaf.Engine.Classes.Decorations;
using WinterLeaf.Engine.Classes.Extensions;
using WinterLeaf.Engine.Classes.Helpers;
using Winterleaf.Demo.Full.Dedicated.Models.User.Extendable;
#endregion

namespace Winterleaf.Demo.Full.Dedicated.Models.Base
    {
    /// <summary>
    /// 
    /// </summary>
    [TypeConverter(typeof(TypeConverterGeneric<GuiFileTreeCtrl_Base>))]
    public partial class GuiFileTreeCtrl_Base: GuiTreeViewCtrl
{

#region ProxyObjects Operator Overrides
        /// <summary>
        /// 
        /// </summary>
        /// <param name="ts"></param>
        /// <param name="simobjectid"></param>
        /// <returns></returns>
        public static bool operator ==(GuiFileTreeCtrl_Base ts, string simobjectid)
            {
            return object.ReferenceEquals(ts, null) ? object.ReferenceEquals(simobjectid, null) : ts.Equals(simobjectid);
            }
  /// <summary>
        /// 
        /// </summary>
        /// <returns></returns>
        public override int GetHashCode()
            {
            return base.GetHashCode();
            }
  /// <summary>
        /// 
        /// </summary>
        /// <param name="obj"></param>
        /// <returns></returns>
        public override bool Equals(object obj)
            {
            
            return (this._ID ==(string)myReflections.ChangeType( obj,typeof(string)));
            }
        /// <summary>
        /// 
        /// </summary>
        /// <param name="ts"></param>
        /// <param name="simobjectid"></param>
        /// <returns></returns>
        public static bool operator !=(GuiFileTreeCtrl_Base ts, string simobjectid)
            {
            if (object.ReferenceEquals(ts, null))
                return !object.ReferenceEquals(simobjectid, null);
            return !ts.Equals(simobjectid);

            }


            /// <summary>
        /// 
        /// </summary>
        /// <param name="ts"></param>
        /// <returns></returns>
        public static implicit operator string( GuiFileTreeCtrl_Base ts)
            {
            if (object.ReferenceEquals(ts, null))
                 return "0";
            return ts._ID;
            }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="ts"></param>
        /// <returns></returns>
        public static implicit operator GuiFileTreeCtrl_Base(string ts)
            {
            uint simobjectid = resolveobject(ts);
           return  (GuiFileTreeCtrl_Base) Omni.self.getSimObject(simobjectid,typeof(GuiFileTreeCtrl_Base));
            }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="ts"></param>
        /// <returns></returns>
        public static implicit operator int( GuiFileTreeCtrl_Base ts)
            {
            return (int)ts._iID;
            }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="simobjectid"></param>
        /// <returns></returns>
        public static implicit operator GuiFileTreeCtrl_Base(int simobjectid)
            {
            return  (GuiFileTreeCtrl) Omni.self.getSimObject((uint)simobjectid,typeof(GuiFileTreeCtrl_Base));
            }


        /// <summary>
        /// 
        /// </summary>
        /// <param name="ts"></param>
        /// <returns></returns>
        public static implicit operator uint( GuiFileTreeCtrl_Base ts)
            {
            return ts._iID;
            }

        /// <summary>
        /// 
        /// </summary>
        /// <returns></returns>
        public static implicit operator GuiFileTreeCtrl_Base(uint simobjectid)
            {
            return  (GuiFileTreeCtrl_Base) Omni.self.getSimObject(simobjectid,typeof(GuiFileTreeCtrl_Base));
            }
#endregion
#region Init Persists
/// <summary>
/// Vector of file patterns. If not empty, only files matching the pattern will be shown in the control. 
/// </summary>
[MemberGroup("File Tree")]
public String fileFilter
       {
       get
          {
          return Omni.self.GetVar(_ID + ".fileFilter").AsString();
          }
       set
          {
          Omni.self.SetVar(_ID + ".fileFilter", value.AsString());
          }
       }
/// <summary>
/// Path in game directory that should be displayed in the control. 
/// </summary>
[MemberGroup("File Tree")]
public String rootPath
       {
       get
          {
          return Omni.self.GetVar(_ID + ".rootPath").AsString();
          }
       set
          {
          Omni.self.SetVar(_ID + ".rootPath", value.AsString());
          }
       }

#endregion
#region Member Functions
/// <summary>
/// getSelectedPath() - returns the currently selected path in the tree)
/// 
/// </summary>
[MemberFunctionConsoleInteraction(true)]
public  string getSelectedPath(){

return pInvokes.m_ts.fn_GuiFileTreeCtrl_getSelectedPath(_ID);
}
/// <summary>
/// () - Reread the directory tree hierarchy. )
/// 
/// </summary>
[MemberFunctionConsoleInteraction(true)]
public  void reload(){

pInvokes.m_ts.fn_GuiFileTreeCtrl_reload(_ID);
}
/// <summary>
/// setSelectedPath(path) - expands the tree to the specified path)
/// 
/// </summary>
[MemberFunctionConsoleInteraction(true)]
public  bool setSelectedPath(string path){

return pInvokes.m_ts.fn_GuiFileTreeCtrl_setSelectedPath(_ID, path);
}

#endregion
#region T3D Callbacks

        /// <summary>
        /// )
        /// </summary>
        [ConsoleInteraction(true)]
public virtual  void onSelectPath(string path){}

#endregion
public GuiFileTreeCtrl_Base (){}
}}
