CNtlTSTrigger
{
	sm = 1;
	sq = 1;
	qc = 6;
	rq = 0;
	tid = 1533;
	title = 153302;

	CNtlTSGroup
	{
		gid = 0;

		CDboTSContGAct
		{
			cid = 2;
			elnk = 252;
			nextlnk = 100;
			prelnk = "1;";

			CDboTSActNPCConv
			{
				conv = 153309;
				ctype = 1;
				idx = 3142116;
				taid = 2;
			}
			CDboTSActRegQInfo
			{
				cont = 153308;
				gtype = 1;
				area = 153301;
				goal = 153304;
				grade = 132203;
				rwd = 100;
				scitem = -1;
				sort = 153305;
				stype = 2;
				taid = 1;
				title = 153302;
			}
		}
		CDboTSContEnd
		{
			cid = 253;
			prelnk = "0;1;252;";
			type = 0;
		}
		CDboTSContEnd
		{
			cid = 254;
			prelnk = "100;";
			type = 1;
		}
		CDboTSContStart
		{
			cid = 0;
			stdiag = 153307;
			nolnk = 253;
			rm = 0;
			yeslnk = 1;

			CDboTSCheckLvl
			{
				maxlvl = 100;
				minlvl = 48;
			}
			CDboTSClickNPC
			{
				npcidx = "3142116;";
			}
			CDboTSCheckClrQst
			{
				and = "1518;";
				flink = 1;
				flinknextqid = "1534;";
				not = 0;
			}
		}
		CDboTSContProposal
		{
			cancellnk = 253;
			cid = 1;
			cont = 153308;
			gtype = 1;
			oklnk = 2;
			area = 153301;
			goal = 153304;
			sort = 153305;
			prelnk = "0;";
			ds = 1;
			grade = 132203;
			rwd = 100;
			title = 153302;
		}
		CDboTSContReward
		{
			canclnk = 255;
			cid = 100;
			rwdzeny = 0;
			desc = 153314;
			nextlnk = 254;
			rwdexp = 0;
			rwdtbl = 153301;
			rwdtype = 0;
			ltime = -1;
			prelnk = "2;";
			usetbl = 1;

			CDboTSClickNPC
			{
				npcidx = "3331205;";
			}
		}
		CDboTSContGAct
		{
			cid = 252;
			elnk = 255;
			nextlnk = 253;
			prelnk = "2;";
		}
	}
}

