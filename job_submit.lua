--[[

 Example lua script demonstrating the SLURM job_submit/lua interface.
 This is only an example, not meant for use in its current form.

 For use, this script should be copied into a file name "job_submit.lua"
 in the same directory as the SLURM configuration file, slurm.conf.

--]]

function slurm_job_submit(job_desc, part_list, submit_uid)
	if job_desc.partition == "compute" then
		slurm.log_user("%s", job_desc.partition)
		for key,value in pairs(job_desc) do
		    slurm.log_user("%s %s", key, value)
		end
		output = assert(io.popen("/etc/slurm-llnl/ccc.sh"))
		for line in output:lines() do
		    slurm.log_user("%s", line)
		end
	end

	return slurm.SUCCESS
end

function slurm_job_modify(job_desc, job_rec, part_list, modify_uid)
	if job_desc.comment == nil then
		local comment = "***TEST_COMMENT***"
		slurm.log_info("slurm_job_modify: for job %u from uid %u, setting default comment value: %s",
				job_rec.job_id, modify_uid, comment)
		job_desc.comment = comment
	end

	return slurm.SUCCESS
end

slurm.log_info("initialized")
return slurm.SUCCESS
