using System;
using System.Threading.Tasks;
using FinancialApp.Grpc;
using GrpcPricing.Protos;

namespace FinancialApp.Services
{
    public class HedgingService : IHedgingService
    {
        private readonly IGrpcPricingClient _pricingClient;

        public HedgingService(IGrpcPricingClient pricingClient)
        {
            _pricingClient = pricingClient;
        }

        public async Task RunHedgingAsync()
        {
            Console.WriteLine("Starting hedging process...");

            // Heartbeat
            var serverInfo = await _pricingClient.GetHeartbeatAsync();
            Console.WriteLine($"Interest Rate: {serverInfo.DomesticInterestRate}");

            // Pricing Input Example
            var input = new PricingInput
            {
                MonitoringDateReached = true,
                Time = 1.0
            };
            input.Past.Add(new PastLines { Value = { 100.0, 101.0 } });
            input.Past.Add(new PastLines { Value = { 102.0, 103.5 } });

            // Price and Deltas
            var output = await _pricingClient.GetPriceAndDeltasAsync(input);
            Console.WriteLine($"Price: {output.Price}");
            Console.WriteLine($"Deltas: {string.Join(", ", output.Deltas)}");
        }
    }
}
