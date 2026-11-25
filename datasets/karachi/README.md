# Karachi demo dataset

This 30-stop model combines the 22 Green Line stations and the Route 04 segment from Nagan Chowrangi to Sindh High Court. The console shows Green Line as **Line 1** and People's Bus Service Route 04 as **Line 4**.

- **Green Line stop order.** Cross-checked the published station sequence against the Karachi transport map. Some station names use familiar local names, such as Guru Mandir for Patel Para. [SMTA route map](https://smta.gos.pk/route-map) · [Green Line station directory](https://busupdate.com/pakistan/karachi/bus-routes/route-green-line/)
- **Route 04.** Uses the order in the service directory. The model combines stops in the same transfer area at Nagan Chowrangi, Guru Mandir, and Numaish. Liaquatabad is listed as Lalukhait in the directory. [SMTA Route 04](https://smta.gos.pk/routes/4)
- **Times and fares.** Each Green Line segment uses 3 minutes and a fare weight of Rs 3. Each Route 04 segment uses 5 minutes and a fare weight of Rs 4. These are demonstration values. The model treats connections as two-way, combines nearby transfer stops, and omits walking and waiting time. Current tickets and trip times should be measured or verified separately.
- **Red Line.** The published Red Line alignment is a future corridor. TransKarachi documents the planned route via Numaish. [TransKarachi Red Line alignment](https://transkarachi.pk/routes/) · [AIIB project status](https://www.aiib.org/en/projects/details/2019/approved/Pakistan-Karachi-Bus-Rapid-Transit.html)

Run from the project root:

```powershell
.\transit_planner.exe --data-dir datasets/karachi
```

Try **Nagan Chowrangi → Numaish**. The fastest route uses the Green Line for 36 minutes. The cheapest follows Route 04 to Guru Mandir, then changes to the Green Line for the final connection. Its fare weight is Rs 31 and it takes 38 minutes. In **8 → 4**, block connection **12** from Five Star Chowrangi to Hyderi. Fastest then changes to the Route 04 corridor, reaching Numaish in 38 minutes with one transfer. Use option **11** to restore the original route. Option **13** saves a snapshot in this dataset folder.
