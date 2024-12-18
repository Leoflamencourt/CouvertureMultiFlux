using GrpcPricing.Protos;
using MarketData;
using ParameterInfo;

namespace FinancialApp.Grpc
{
    /// <summary>
    /// Interface pour définir un pricer capable de sérialiser les entrées et d'obtenir des sorties via gRPC.
    /// </summary>
    public interface IPricer
    {
       
        Task<PricingOutput> PriceandDeltaAsync(List<DataFeed> dataFeeds, DateTime currentDate, TestParameters testParameters);
    }
}
