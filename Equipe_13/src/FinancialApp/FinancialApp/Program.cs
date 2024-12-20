using System;
using System.Threading.Tasks;
using FinancialApp.Grpc;
using FinancialApp.Services;

using ParameterInfo;
using FinancialApp.Utils.Serialization;
using MarketData;
class Program
{
    static async Task Main(string[] args)
    {
        // Vérifier que les arguments de la ligne de commande sont fournis
        if (args.Length < 3)
        {
            Console.WriteLine("Usage: FinancialApp <marketDataPath> <financialParamsPath>");
            return;
        }
        //Recup filePath
        string financialParamsPath = args[0];
        string marketDataPath = args[1];
        string portfolioPath=args[2];
        Console.WriteLine("Test log - xsjksd:; de l'exécution");
        //read Data
        ParameterJsonDeserializer jsonDeserializer = new ParameterJsonDeserializer();
        TestParameters financialParams = jsonDeserializer.Deserialize(financialParamsPath);

        MarketDataDeserializer csvDeserializer = new MarketDataDeserializer();
        List<DataFeed> dataFeeds = csvDeserializer.Deserialize(marketDataPath);
        HedgingEngine hedgingEngine = new HedgingEngine();

        List<OutputData> outputDatas= await  hedgingEngine.ComputePortfolio(financialParams, dataFeeds );
        


        //write result 
        JsonWriter.writer(portfolioPath, outputDatas);

        
    }
}
