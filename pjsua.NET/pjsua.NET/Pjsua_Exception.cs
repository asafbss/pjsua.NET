using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace pjsua.NET
{
    class Pjsua_Exception: Exception
    {
        public Pjsua_Exception(string message) :
            base(message) { }
    }
}
