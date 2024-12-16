namespace FinancialApp.Services
{
    public interface IHedgingEngineClient
    {
        Task ProcessAsync(string marketDataPath, string financialParamsPath);
        Task RunHedgingAsync(string marketDataPath, string financialParamsPath);
    }
}
