using ParameterInfo.JsonUtils;
using ParameterInfo;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace FinancialApp.Utils.Serialization
{    public static class JsonWriter
    {
        public static void writer(string path, List<OutputData> output)
        {
            string jsonString = JsonIO.ToJson(output);
            File.WriteAllText(path, jsonString);
        }

    }
    
}
