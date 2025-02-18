# vim: filetype=sh:
# This is a comment
# Keywords are case-sensitive

title "Advection of 2D Gaussian hump"

inciter

  nstep 10  # Max number of time steps
  dt   1.0e-3 # Time step size
  ttyi 1     # TTY output interval
  scheme dg
  pelocal_reorder true

  partitioning
    algorithm mj
  end

  transport
    physics advection
    problem gauss_hump
    ncomp 1

    bc_extrapolate
      sideset 1 end
    end
    bc_inlet
      sideset 2 end
    end
    bc_outlet
      sideset 3 end
    end

  end

  amr
   dtref true
   dtref_uniform true
   dtfreq 5
   refvar c end
   error jump
  end

  diagnostics
    interval  2
    format    scientific
    error l2
  end

  field_output
    interval 1
    var elem analytic C1 "c0_numerical" end
  end

end
