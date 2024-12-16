using ParameterInfo;
using ParameterInfo.JsonUtils;
namespace FinancialApp.Deserializer;

public interface IInputParametersJsonDeserializer
{
    public TestParameters Deserialize(string filePath);
}
