

docker: docker_build docker_run
docker_build:
	docker build -t sst_11 .

docker_run:
	docker run -it --rm \
             -v `pwd`:/scratch \
             --user $(id -u):$(id -g) \
             sst_11 /bin/bash 

singularity:
	singularity build sst_image.def
