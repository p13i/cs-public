#!/usr/bin/env bash

#########################################
### DOCKER-SPECIFIC UTILITY FUNCTIONS ###
#########################################

# Finds the name of an active container with the given service name.
function get_service_container_name() {
    # Get the name of the directory (not full path) of where this is run
    # We need to lowercase this directory (as docker-compose does) to be
    # able to properly search for the target container.
    local current_dir=$(echo "${PWD##*/}" | awk '{print tolower($0)}')

    # This is an argument from the user (e.g. api, postgres, etc.)
    local service_name=$1

    # This is how docker-compose constructs the name of compose containers.
    # docker-compose prepends the current directory (without underscores) and service name.
    local docker_compose_prefix="${current_dir//_/}_${service_name}"

    output=$(docker ps --format "{{ .Names }}" | grep ${docker_compose_prefix} | sed -n 1p)

    if [ -z "$output" ]; then
        # If there is no output, then we are probably dealing the the name of a fixed container.
        # Try returning just the results for using the service_name
        echo $(docker ps --format "{{ .Names }}" | grep ${service_name} | sed -n 1p)
    else
        echo ${output}
    fi
}


# Checks if a given volume exists
# Usage: does_volume_exist VOLUME_NAME
# Argument VOLUME_NAME: The name of the volume to check if exists
# Return: Boolean (int) of whether or not the volume of the given name exists or not
function does_volume_exist() {
    local VOLUME_NAME=$1
    if [[ -z $(docker volume ls | grep $VOLUME_NAME) ]]; then
        # There is no volume with the given name
        echo "0";
    else
        echo "1";
    fi
}


# Creates volumes if they do not exist
# Usage: create_volumes_if_not_exist VOLUME_NAME_1 VOLUME_NAME_2 ...
# Argument(s) VOLUME_NAME_1 VOLUME_NAME_2 ...: The names of volumes to create if they do not exist
# Return: Nothing
function create_volumes_if_not_exist() {
    echo "Creating volumes if they do not exist:"

    for VOLUME_NAME in ${@:1}
    do
        volume_exists="$(does_volume_exist $VOLUME_NAME)"
        if [ "$volume_exists" -eq "0" ]; then

            echo "--> Creating non-existent volume $VOLUME_NAME."
            docker volume create $VOLUME_NAME

        else

            echo "--> Volume $VOLUME_NAME already exists. Skipping creation."

        fi
    done

    echo "Done."
    echo ""
}

# Removes volumes if they do exist
# Usage: remove_volumes VOLUME_NAME_1 VOLUME_NAME_2 ...
# Argument(s) VOLUME_NAME_1 VOLUME_NAME_2 ...: The names of volumes to remove if they exist
# Return: Nothing
function remove_volumes() {
    echo "Removing volumes if they exist:"

    for VOLUME_NAME in ${@:1}
    do
        volume_exists="$(does_volume_exist $VOLUME_NAME)"
        if [ "$volume_exists" -eq "0" ]; then

            echo "--> Volume $VOLUME_NAME doesn't exist. Skipping removal."

        else

            echo "--> Volume $VOLUME_NAME exists. Removing..."
            docker volume rm $VOLUME_NAME
            echo "    --> Removed $VOLUME_NAME."

        fi
    done

    echo "Done."
    echo ""
}
