using FinancialApp.Deserializer;
using ParameterInfo;


namespace FinancialApp.Parser
{
    public class ParameterJsonDeserialiser : IInputParametersJsonDeserializer
    {
        public TestParameters Deserialize(string filePath)
        {
            string jsonContent=File.ReadAllText(filePath);
            return ParameterInfo.JsonUtils.JsonIO.FromJson(jsonContent);
        }
    }
}
