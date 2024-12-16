using System;
using System.Threading.Tasks;
using FinancialApp.Grpc;
using FinancialApp.Services;

namespace FinancialApp
{
    class Program
    {
        static async Task Main(string[] args)
        {
            Console.WriteLine("Initializing Financial Application...");

            // Dependency Injection
            string serverAddress = "http://localhost:5079";
            IGrpcPricingClient grpcClient = new GrpcPricingClient(serverAddress);
            IHedgingService hedgingService = new HedgingService(grpcClient);

            // Run the Hedging Process
            await hedgingService.RunHedgingAsync();

            Console.WriteLine("Hedging process completed.");
        }
    }
}
