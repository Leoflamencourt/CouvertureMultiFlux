namespace FinancialApp.Utils.Serialization
{
    public interface ISerializer<TInput, TOutput>
    {
        TOutput Serialize(TInput input, bool monitoringDateReached, DateTime time);
    }
}
