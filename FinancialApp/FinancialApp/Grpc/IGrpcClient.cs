using System.Threading.Tasks;
using GrpcPricing.Protos;

namespace FinancialApp.Grpc
{
    public interface IGrpcPricingClient
    {
        Task<ReqInfo> GetHeartbeatAsync();
        Task<PricingOutput> GetPriceAndDeltasAsync(PricingInput input);
    }
}
