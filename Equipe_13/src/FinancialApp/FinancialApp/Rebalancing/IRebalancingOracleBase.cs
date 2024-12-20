using ParameterInfo.RebalancingOracleDescriptions;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace FinancialApp.Rebalancing
{
    public abstract class IRebalancingOracleBase
    {
        public RebalancingOracleType OracleType { get; protected set; }

        public abstract bool ShouldRebalance(DateTime currentDate);
    }
}
