using ParameterInfo;

namespace FinancialApp.Utils.Serialization
{
    public interface ISerializer<TInput, TOutput>
    {
        TOutput Serialize(TInput input, TestParameters parameters);
    }
}
