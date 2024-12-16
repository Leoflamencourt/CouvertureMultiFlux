using GrpcPricing.Protos;
using MarketData;
namespace FinancialApp.Serializer
{
    public interface IPricingInputSerializer
    {
        PricingInput Serialize(List<DataFeed> marketData, bool monitoringDateReached, double time);
    }
}
