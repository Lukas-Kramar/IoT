import { Dispatch } from "react";
import { Col, Form, Row } from "react-bootstrap";

import DefaultModal from "../../../components/modals/DefaultModal";

import { useLoggedUserContext } from '../../../customHooks/useLoggedUserContext';
import { useOrganisationContext } from "../../../customHooks/useOrganisationsContext";

interface Props {
    modalVersion: 'create-organisation',
    setModalVersion: Dispatch<string>,
}

const CreateOrganisation = (props: Props) => {
    const { modalVersion, setModalVersion } = props;

    const { userData } = useLoggedUserContext();
    const { addOrganisations } = useOrganisationContext();

    const handleSubmitNewProject = async () => {
        if (!userData) {
            console.error("handleSubmitNewProject - userData is null: ", userData);
            return;
        }

        // TODO - write logic for creating new Organisation
        addOrganisations([]); // TODO insert new organisation into the list
        setModalVersion("")
    }


    if (modalVersion !== 'create-organisation') { return null; }
    return (
        <DefaultModal
            show={modalVersion === "create-organisation"}
            onHide={() => setModalVersion('')}
            onSubmit={() => { }}
            submitText="Create Organisation"
            title={"Create Organisation"}
        >
            <Form>
                <Form.Group as={Row} >
                    <Form.Label column sm={4}>
                        XXX
                    </Form.Label>
                    <Col sm={8}>
                        <Form.Control type="text" placeholder="XXX" />
                    </Col>
                </Form.Group>

                <Form.Group as={Row} className="mt-4">
                    <Form.Label column sm={4}>
                        XXX
                    </Form.Label>
                    <Col sm={8} className="">

                    </Col>
                </Form.Group>
            </Form>

        </DefaultModal>
    );
}

export default CreateOrganisation;