using System.Threading.Tasks;

namespace FinancialApp.Services
{
    public interface IHedgingService
    {
        Task RunHedgingAsync();
    }
}
