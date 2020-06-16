  <!-- Header -->
  <header class="masthead d-flex">
    <div class="container text-center my-auto">
      <h1 class="mb-1">My Virtual Machine </h1>
      <h3 class="mb-5">
        <em>Interact with your Virtual Machine remotely now</em>
      </h3>
      <a class="btn btn-primary btn-xl js-scroll-trigger" href="#dashboard">Get Started</a>
    </div>
    <div class="overlay"></div>
  </header>
  <!-- Portfolio -->
  <section class="content-section" id="dashboard">
    <div class="container">
      <div class="content-section-heading text-center">
        <h3 class="text-secondary mb-0">Run Your VM</h3>
        <h2 class="mb-5">Our Features</h2>
      </div>
      <div class="row no-gutters">
        <div class="col-lg-6">
          <a class="portfolio-item command" name="on"  id="On">
            <span class="caption">
              <span class="caption-content">
                <h2>Turn ON</h2>
                <p class="mb-0">Turn on your VM remotely</p>
              </span>
            </span>
            <img class="img-features" src="<?php echo $assets_url; ?> /img/turn_on.jpg" alt="">
          </a>
        </div>
        <div class="col-lg-6">
          <a class="portfolio-item command"  name="off" id="Off">
            <span class="caption">
              <span class="caption-content">
                <h2>Turn OFF</h2>
                <p class="mb-0">Turn off your VM remotely</p>
              </span>
            </span>
            <img class="img-features" src="<?php echo $assets_url; ?> /img/turn_off.jpg" alt="">
          </a>
        </div>
        <div class="col-lg-6">
          <a class="portfolio-item command"  name="suspend"  id="Suspend">
            <span class="caption">
              <span class="caption-content">
                <h2>Suspend</h2>
                <p class="mb-0">Suspend your VM remotely</p>
              </span>
            </span>
            <img class="img-features" src="<?php echo $assets_url; ?> /img/suspend.jpg" alt="">
          </a>
        </div>
        <div class="col-lg-6">
          <a class="portfolio-item open-scriptDialog"  name="script" id="Script" data-toggle="modal" data-target="#scriptModal">
            <span class="caption">
              <span class="caption-content">
                <h2>Script</h2>
                <p class="mb-0">Run Your Script to VM remotely, Upload Now</p>
              </span>
            </span>
            <img class="img-features" src="<?php echo $assets_url; ?>/img/script.jpg" alt="">
          </a>
        </div>
        <div class="col-lg-6">
          <a class="portfolio-item open-modalDialog"   name="fclone" id="Fclone" data-toggle="modal" data-target="#cloneModal">
            <span class="caption">
              <span class="caption-content">
                <h2>Full Clone</h2>
                <p class="mb-0">Full Clone your VM remotely, Upload Now</p>
              </span>
            </span>
            <img class="img-features" src="<?php echo $assets_url; ?>/img/clone.jpg" alt="">
          </a>
        </div>
        <div class="col-lg-6">
          <a class="portfolio-item open-modalDialog"  name="lclone" id="Lclone"  data-toggle="modal" data-target="#cloneModal">
            <span class="caption">
              <span class="caption-content">
                <h2>Linked Clone</h2>
                <p class="mb-0">Linked Clone your VM remotely, Upload Now</p>
              </span>
            </span>
            <img class="img-features" src="<?php echo $assets_url; ?>/img/linked_clone.jpg" alt="">
          </a>
        </div>
        <div class="col-lg-6">
          <a class="portfolio-item open-runDialog"  name="run_program" id="RunProgram"  data-toggle="modal" data-target="#runModal">
            <span class="caption">
              <span class="caption-content">
                <h2>Run Program</h2>
                <p class="mb-0">Run program on your VM remotely </p>
              </span>
            </span>
            <img class="img-features" src="<?php echo $assets_url; ?>/img/run.jpg" alt="">
          </a>
        </div>
        <div class="col-lg-6">
          <a class="portfolio-item open-snapshotDialog"  name="create_snapshot" id="Snapshot"  data-toggle="modal" data-target="#snapshotModal">
            <span class="caption">
              <span class="caption-content">
                <h2>Snapshot</h2>
                <p class="mb-0">Create a snapshot on your current state</p>
              </span>
            </span>
            <img class="img-features" src="<?php echo $assets_url; ?>/img/camera.jpg" alt="">
          </a>
        </div>
        <div class="col-lg-6">
          <a class="portfolio-item open-revertDialog"  name="revert_snapshot" id="Revert"  data-toggle="modal" data-target="#revertModal">
            <span class="caption">
              <span class="caption-content">
                <h2>Revert Snapshot</h2>
                <p class="mb-0">Back to the state before</p>
              </span>
            </span>
            <img class="img-features" src="<?php echo $assets_url; ?>/img/revert.jpg" alt="">
          </a>
        </div>
        <!-- Clone Modal -->
        <div class="modal fade" id="cloneModal" tabindex="-1" role="dialog" aria-labelledby="clonemodal" aria-hidden="true">
          <div class="modal-dialog" role="document">
            <div class="modal-content">
              <div class="modal-header">
                <h5 class="modal-title" id="exampleModalLabel">CLONE NAME</h5>
                <button type="button" class="close" data-dismiss="modal" aria-label="Close">
                  <span aria-hidden="true">&times;</span>
                </button>
              </div>
              <div class="modal-body">
                  <input class="form-control" id="clone_name" type="text" name="clone_name" placeholder="Name your clone...">
              </div>
              <div class="modal-footer">
                <button type="button" class="btn btn-secondary" data-dismiss="modal">Close</button>
                <button type="button" class="btn btn-primary command cloneModal" data-dismiss="modal">CLONE</button>
              </div>
            </div>
          </div>
        </div>
        <!-- Script Modal -->
        <div class="modal fade" id="scriptModal" tabindex="-1" role="dialog" aria-labelledby="scriptmodal" aria-hidden="true">
          <div class="modal-dialog" role="document">
            <div class="modal-content">
              <div class="modal-header">
                <h5 class="modal-title" id="exampleModalLabel">SCRIPT FILE</h5>
                <button type="button" class="close" data-dismiss="modal" aria-label="Close">
                  <span aria-hidden="true">&times;</span>
                </button>
              </div>
              <div class="modal-body">
                  <input class="form-control" type="file" name="file_script" id="file_script">
              </div>
              <div class="modal-footer">
                <button type="button" class="btn btn-secondary" data-dismiss="modal">Close</button>
                <button type="button" class="btn btn-primary command scriptModal" data-dismiss="modal">RUN</button>
              </div>
            </div>
          </div>
        </div>
        <div class="modal fade" id="runModal" tabindex="-1" role="dialog" aria-labelledby="runmodal" aria-hidden="true">
          <div class="modal-dialog" role="document">
            <div class="modal-content">
              <div class="modal-header">
                <h5 class="modal-title" id="exampleModalLabel">RUN COMMAND</h5>
                <button type="button" class="close" data-dismiss="modal" aria-label="Close">
                  <span aria-hidden="true">&times;</span>
                </button>
              </div>
              <div class="modal-body">
                  <input class="form-control" id="run_command" type="text" name="run_command" placeholder="Fill the command ">
                  <input class="form-control mt-2" id="arguments" type="text" name="arguments" placeholder="Fill the arguments">
              </div>
              <div class="modal-footer">
                <button type="button" class="btn btn-secondary" data-dismiss="modal">Close</button>
                <button type="button" class="btn btn-primary command runModal" data-dismiss="modal">RUN</button>
              </div>
            </div>
          </div>
        </div>
        <div class="modal fade" id="snapshotModal" tabindex="-1" role="dialog" aria-labelledby="snapshotmodal" aria-hidden="true">
          <div class="modal-dialog" role="document">
            <div class="modal-content">
              <div class="modal-header">
                <h5 class="modal-title" id="exampleModalLabel">SNAPSHOT DATA</h5>
                <button type="button" class="close" data-dismiss="modal" aria-label="Close">
                  <span aria-hidden="true">&times;</span>
                </button>
              </div>
              <div class="modal-body">
                <input class="form-control" id="name_snapshot" type="text" name="name_snapshot" placeholder="Name of the snapshot ">
                <input class="form-control mt-2" id="description" type="text" name="description" placeholder="Description">
              </div>
              <div class="modal-footer">
                <button type="button" class="btn btn-secondary" data-dismiss="modal">Close</button>
                <button type="button" class="btn btn-primary command ssModal" data-dismiss="modal">RUN</button>
              </div>
            </div>
          </div>
        </div>
        <div class="modal fade" id="revertModal" tabindex="-1" role="dialog" aria-labelledby="snapshotmodal" aria-hidden="true">
          <div class="modal-dialog" role="document">
            <div class="modal-content">
              <div class="modal-header">
                <h5 class="modal-title" id="exampleModalLabel">REVERT SNAPSHOT DATA</h5>
                <button type="button" class="close" data-dismiss="modal" aria-label="Close">
                  <span aria-hidden="true">&times;</span>
                </button>
              </div>
              <div class="modal-body">
                <input class="form-control" id="revert_name_snapshot" type="text" name="revert_name_snapshot" placeholder="Name of the snapshot ">
              </div>
              <div class="modal-footer">
                <button type="button" class="btn btn-secondary" data-dismiss="modal">Close</button>
                <button type="button" class="btn btn-primary command rsModal" data-dismiss="modal">RUN</button>
              </div>
            </div>
          </div>
        </div>
      </div>
    </div>
  </section>
