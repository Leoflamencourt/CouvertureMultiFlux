using ParameterInfo;
using ParameterInfo.JsonUtils;

namespace FinancialApp.Utils.Serialization
{
    public class ParameterJsonDeserializer : IDeserializer<TestParameters>
    {
        public TestParameters Deserialize(string filePath)
        {
            string jsonContent = File.ReadAllText(filePath);
            return JsonIO.FromJson(jsonContent);
        }
    }
}
